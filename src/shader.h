#ifndef SHADER_H
#define SHADER_H

#include <QString>

class Shader
{
public:
	Shader(const QString& vert, const QString& frag);
	~Shader();

	void bind();
	static void unbind();
	
	int uniformLocation(const QString& name);
	bool isCurrent() const;

private:
	uint loadShader(uint program, int type, const QString& filename);
	
	void load();
	void free();
	
	QString m_vertFilename;
	QString m_fragFilename;
	
	uint m_program;
	uint m_vert;
	uint m_frag;
	
	static Shader* s_current;
};

#endif
