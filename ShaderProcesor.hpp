#ifndef SHADERPROCESOR_H
#define SHADERPROCESOR_H

class ShaderProcesor  // klasa bazowa wszystkich procesorów
{
	public:
		virtual ~ShaderProcesor() {};
		virtual void run(BITMAP* in,BITMAP* out) = 0;  // czysto wirtualna metoda do uruchamiania filtra
	protected:
		friend class ShaderEngine;
	private:
};



#endif // SHADERPROCESOR_H
