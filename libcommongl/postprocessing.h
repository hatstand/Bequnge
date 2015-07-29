#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H

#include "opengl.h"
#include "vector.h"

#include <QList>
#include <boost/function.hpp>

class QGLFramebufferObject;
class Shader;

typedef boost::function<void ()> BloomCallback;

class PostProcessing
{
public:
	PostProcessing();
	~PostProcessing();
	
	void init();
	void windowResized(int width, int height);
	
	void setBloomCallback(const BloomCallback& cb) { m_bloomCallback = cb; }
	
	void prepareBuffers();
	void run();
	
	void sceneReadPixels(int x, int y, int width, int height, GLenum format, GLenum type, void* pixels) const;
	
	void setBloomAmount(float bloomAmount) { m_bloomAmount = bloomAmount; }
	void setMotionBlurAmount(float motionBlurAmount) { m_motionBlurAmount = motionBlurAmount; }
	
	float bloomAmount() const { return m_bloomAmount; }
	float motionBlurAmount() const { return m_motionBlurAmount; }
	
	int borderSize() const { return k_borderSize; }
	
private:
	void initTarget(QGLFramebufferObject** fbo, const QSize& size, bool depthBuffer = false);
	void drawQuad(const vec2& origin = vec2(0.0, 0.0), const vec2& size = vec2(1.0, 1.0));
	void blur(QGLFramebufferObject* fbo);
	void shaderPass(Shader* shader, QGLFramebufferObject* source, QGLFramebufferObject* target);
	
	int m_width;
	int m_height;
	
	Shader* m_shaderDownsample;
	Shader* m_shaderMotionBlur;
	Shader* m_shaderBlurY;
	Shader* m_shaderBlurX;
	Shader* m_shaderBlend;
	Shader* m_shaderBloom;
	
	int m_currentSceneFbo;
	QGLFramebufferObject* m_sceneFbo[2];
	QGLFramebufferObject* m_blurTarget[3];
	QGLFramebufferObject* m_bloomTarget[3];
	
	int m_previousFrameLoc;
	int m_sceneLoc;
	int m_blur1Loc;
	int m_blur2Loc;
	int m_blur3Loc;
	int m_blur1StepLoc;
	int m_blur2StepLoc;
	int m_blur3StepLoc;
	
	int m_bloomAmountLoc;
	int m_motionBlurAmountLoc;
	int m_desaturationAmountLoc;
	
	vec2 m_blurStep[3];
	
	float m_bloomAmount;
	float m_motionBlurAmount;
	float m_desaturationAmount;
	
	BloomCallback m_bloomCallback;
	
	static const int k_borderSize;
};

#endif
