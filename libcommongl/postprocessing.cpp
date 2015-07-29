#include "postprocessing.h"
#include "shader.h"
#include "commonutils.h"

#include <QGLFramebufferObject>

const int PostProcessing::k_borderSize = 16;

PostProcessing::PostProcessing()
	: m_currentSceneFbo(0),
	  m_bloomAmount(1.0),
	  m_motionBlurAmount(0.2),
	  m_desaturationAmount(0.0),
	  m_bloomCallback(NULL)
{
	m_sceneFbo[0] = NULL;
	m_sceneFbo[1] = NULL;
	for (int i=0 ; i<3 ; ++i)
	{
		m_blurTarget[i] = NULL;
		m_bloomTarget[i] = NULL;
	}
}

PostProcessing::~PostProcessing()
{
	for (int i=0 ; i<2 ; ++i)
		delete m_sceneFbo[i];
	
	for (int i=0 ; i<3 ; ++i)
		delete m_blurTarget[i];
	
	for (int i=0 ; i<3 ; ++i)
		delete m_bloomTarget[i];
	
	//delete m_shaderDownsample;
	delete m_shaderMotionBlur;
	delete m_shaderBlurY;
	delete m_shaderBlurX;
	delete m_shaderBlend;
	delete m_shaderBloom;
}

void PostProcessing::init()
{
	//m_shaderDownsample = new Shader(":/libcommongl/shaders/pp_vert.glsl", ":/libcommongl/shaders/pp_downsample.glsl");
	m_shaderMotionBlur = new Shader(":/libcommongl/shaders/pp_vert.glsl", ":/libcommongl/shaders/pp_motionblur.glsl");
	m_shaderBlurY = new Shader(":/libcommongl/shaders/pp_vert.glsl", ":/libcommongl/shaders/pp_blury.glsl");
	m_shaderBlurX = new Shader(":/libcommongl/shaders/pp_vert.glsl", ":/libcommongl/shaders/pp_blurx.glsl");
	m_shaderBlend = new Shader(":/libcommongl/shaders/pp_vert.glsl", ":/libcommongl/shaders/pp_blend.glsl");
	m_shaderBloom = new Shader(":/libcommongl/shaders/pp_vert.glsl", ":/libcommongl/shaders/pp_bloom.glsl");
	
	m_previousFrameLoc = m_shaderMotionBlur->uniformLocation("previousFrame");
	m_sceneLoc = m_shaderBlend->uniformLocation("scene");
	m_blur1Loc = m_shaderBlend->uniformLocation("blur1");
	m_blur2Loc = m_shaderBlend->uniformLocation("blur2");
	m_blur3Loc = m_shaderBlend->uniformLocation("blur3");
	
	/*m_blur1StepLoc = m_shaderBlend->uniformLocation("blur1Step");
	m_blur2StepLoc = m_shaderBlend->uniformLocation("blur2Step");
	m_blur3StepLoc = m_shaderBlend->uniformLocation("blur3Step");*/
	
	m_bloomAmountLoc = m_shaderBlend->uniformLocation("bloomAmount");
	m_motionBlurAmountLoc = m_shaderMotionBlur->uniformLocation("motionBlurAmount");
	m_desaturationAmountLoc = m_shaderBlend->uniformLocation("desaturationAmount");
}

void PostProcessing::windowResized(int width, int height)
{
	m_width = width;
	m_height = height;
	
	QSize size(nextPowerOf2(width + k_borderSize*2), nextPowerOf2(height + k_borderSize*2));
	
	for (int i=0 ; i<2 ; ++i)
	{
		delete m_sceneFbo[i];
		m_sceneFbo[i] = new QGLFramebufferObject(size.width(), size.height(), QGLFramebufferObject::Depth);
		glBindTexture(GL_TEXTURE_2D, m_sceneFbo[i]->texture());
		
		m_sceneFbo[i]->bind();
		glViewport(0, 0, size.width(), size.height());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_sceneFbo[i]->release();
	}
	
	for (int i=0 ; i<3 ; ++i)
	{
		size /= 2;
		
		//initTarget(&m_blurTarget[i], size);
		initTarget(&m_bloomTarget[i], size, i == 0);
		
		m_blurStep[i] = 1.0f / vec2(size);
	}
}

void PostProcessing::initTarget(QGLFramebufferObject** fbo, const QSize& size, bool depthBuffer)
{
	delete (*fbo);
	(*fbo) = new QGLFramebufferObject(size, depthBuffer ? QGLFramebufferObject::Depth : QGLFramebufferObject::NoAttachment);
	glBindTexture(GL_TEXTURE_2D, (*fbo)->texture());
	
	(*fbo)->bind();
	glViewport(0, 0, size.width(), size.height());
	glClear(GL_COLOR_BUFFER_BIT);
	(*fbo)->release();
}

void PostProcessing::prepareBuffers()
{
	m_sceneFbo[m_currentSceneFbo]->bind();
	glViewport(0, 0, m_width + k_borderSize*2, m_height + k_borderSize*2);
}

void PostProcessing::run()
{
	const vec2 sceneSize(float(m_width) / m_sceneFbo[m_currentSceneFbo]->width(),
	                     float(m_height) / m_sceneFbo[m_currentSceneFbo]->height());
	const vec2 sceneOrigin(float(k_borderSize) / m_sceneFbo[m_currentSceneFbo]->width(),
	                       float(k_borderSize) / m_sceneFbo[m_currentSceneFbo]->height());
	
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	
	// Do motion blur
	m_shaderMotionBlur->bind();
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, m_sceneFbo[!m_currentSceneFbo]->texture());
	glUniform1i(m_previousFrameLoc, 0);
	glUniform1f(m_motionBlurAmountLoc, m_motionBlurAmount);
	
	drawQuad(vec2(0.0, 0.0), vec2(m_width + k_borderSize*2, m_height + k_borderSize*2));
	
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	Shader::release();
	
	m_sceneFbo[m_currentSceneFbo]->release();
	
	// Get the bright bits out of the scene
	if (!m_bloomCallback)
		shaderPass(m_shaderBloom, m_sceneFbo[m_currentSceneFbo], m_bloomTarget[0]);
	else
	{
		glViewport(0, 0, (m_width + k_borderSize*2)/2, (m_height + k_borderSize*2)/2);
		m_bloomTarget[0]->bind();
		m_bloomCallback();
		m_bloomTarget[0]->release();
	}
	
	// Downsample and blur
	shaderPass(m_shaderBlurX, m_bloomTarget[0], m_bloomTarget[0]);
	shaderPass(m_shaderBlurY, m_bloomTarget[0], m_bloomTarget[0]);
	
	shaderPass(m_shaderBlurX, m_bloomTarget[0], m_bloomTarget[1]);
	shaderPass(m_shaderBlurY, m_bloomTarget[1], m_bloomTarget[1]);
	
	shaderPass(m_shaderBlurX, m_bloomTarget[1], m_bloomTarget[2]);
	shaderPass(m_shaderBlurY, m_bloomTarget[2], m_bloomTarget[2]);
	
	// Draw back to the screen
	glViewport(0, 0, m_width, m_height);
	m_shaderBlend->bind();
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_sceneFbo[m_currentSceneFbo]->texture());
	
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_bloomTarget[0]->texture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_bloomTarget[1]->texture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_bloomTarget[2]->texture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glUniform1i(m_sceneLoc, 0);
	glUniform1i(m_blur1Loc, 1);
	glUniform1i(m_blur2Loc, 2);
	glUniform1i(m_blur3Loc, 3);
	glUniform1f(m_bloomAmountLoc, m_bloomAmount);
	glUniform1f(m_desaturationAmountLoc, m_desaturationAmount);
	/*glUniform2f(m_blur1StepLoc, m_blurStep[0]);
	glUniform2f(m_blur2StepLoc, m_blurStep[1]);
	glUniform2f(m_blur3StepLoc, m_blurStep[2]);*/
	
	drawQuad(sceneOrigin, sceneSize);
	
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDepthMask(true);
	
	Shader::release();
	
	m_currentSceneFbo = !m_currentSceneFbo;
}

void PostProcessing::blur(QGLFramebufferObject* fbo)
{
	shaderPass(m_shaderBlurY, fbo, fbo);
	shaderPass(m_shaderBlurX, fbo, fbo);
}

void PostProcessing::shaderPass(Shader* shader, QGLFramebufferObject* source, QGLFramebufferObject* target)
{
	glViewport(0, 0, target->size());
	target->bind();
	
	shader->bind();
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, source->texture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glUniform1i(shader->uniformLocation("source"), 0);
	glUniform2f(shader->uniformLocation("pixelStep"), 1.0 / target->width(), 1.0 / target->height());
	drawQuad();
	glDisable(GL_TEXTURE_2D);
	
	target->release();
}

void PostProcessing::drawQuad(const vec2& origin, const vec2& size)
{
	glBegin(GL_QUADS);
		glMultiTexCoord2f(GL_TEXTURE0, origin.x, origin.y + size.y);
		glMultiTexCoord2f(GL_TEXTURE1, 0.0, 1.0);
		glVertex2f(-1.0, 1.0);
		
		glMultiTexCoord2f(GL_TEXTURE0, origin.x + size.x, origin.y + size.y);
		glMultiTexCoord2f(GL_TEXTURE1, 1.0, 1.0);
		glVertex2f(1.0, 1.0);
		
		glMultiTexCoord2f(GL_TEXTURE0, origin.x + size.x, origin.y);
		glMultiTexCoord2f(GL_TEXTURE1, 1.0, 0.0);
		glVertex2f(1.0, -1.0);
		
		glMultiTexCoord2f(GL_TEXTURE0, origin.x, origin.y);
		glMultiTexCoord2f(GL_TEXTURE1, 0.0, 0.0);
		glVertex2f(-1.0, -1.0);
	glEnd();
}

void PostProcessing::sceneReadPixels(int x, int y, int width, int height, GLenum format, GLenum type, void* pixels) const
{
	m_sceneFbo[!m_currentSceneFbo]->bind();
	
	glReadPixels(x, y, width, height, format, type, pixels);
	
	m_sceneFbo[!m_currentSceneFbo]->release();
}

