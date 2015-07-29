#include "shader.h"
#include "opengl.h"

#include <QFile>
#include <QtDebug>

Shader* Shader::s_current;

Shader::Shader(const QString& vert, const QString& frag)
	: m_vertFilename(vert),
	  m_fragFilename(frag)
{
	load();
}

Shader::~Shader()
{
	free();
}

void Shader::load()
{
	m_program = glCreateProgram();
	m_vert = loadShader(m_program, GL_VERTEX_SHADER, m_vertFilename);
	m_frag = loadShader(m_program, GL_FRAGMENT_SHADER, m_fragFilename);
	
	glLinkProgram(m_program);
	
	//qDebug() << "Loaded shader pair:" << m_vertFilename << "," << m_fragFilename;
}

void Shader::free()
{
	if (glIsShader(m_vert))
		glDeleteShader(m_vert);
	if (glIsShader(m_frag))
		glDeleteShader(m_frag);
	if (glIsProgram(m_program))
		glDeleteProgram(m_program);
}

uint Shader::loadShader(uint program, int type, const QString& filename)
{
	// Load the source file
	QFile file(filename);
	if (!file.exists())
		qFatal("Shader source not found %s\n", qPrintable(filename));
	
	file.open(QIODevice::ReadOnly);
	QByteArray source(file.readAll());
	
	// Create the shader
	uint ret = glCreateShader(type);
	
	const char* data[1];
	data[0] = source.data();
	glShaderSource(ret, 1, data, NULL);
	
	// Compile the shader
	glCompileShader(ret);
	
	// Get the log
	int logLength;
	glGetShaderiv(ret, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 1)
	{
		char* log = new char[logLength];
		glGetShaderInfoLog(ret, logLength, NULL, log);
		qDebug() << "Compilation log for " << filename << ":" << log;
		delete[] log;
	}
	
	// Check if it was ok
	int success;
	glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
	if (!success)
		qFatal("Error compiling GLSL %s\n", qPrintable(filename));
	
	// Attach it to the program
	glAttachShader(program, ret);
	
	return ret;
}

void Shader::bind()
{
	glUseProgram(m_program);
	s_current = this;
}

void Shader::release()
{
	glUseProgram(0);
	s_current = NULL;
}

int Shader::uniformLocation(const QString& name)
{
	return glGetUniformLocation(m_program, qPrintable(name));
}

bool Shader::isCurrent() const
{
	return s_current == this;
}
