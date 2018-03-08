#ifndef SHADERENGINE_H
#define SHADERENGINE_H

#include "ShaderProcesor.hpp"
#include <vector>

class ShaderEngine   // kontener na filtry
{
	public:
		typedef std::vector<ShaderProcesor*> TProcesors;  // typ tablicy obiektów klasy ShaderProcesor


		// kontruktor z filtra
		template<typename TShader>
		ShaderEngine(const TShader& shader)
		{
			procesors.push_back( new typename TShader::TShaderProcesor(shader) ); //dodaj to tablicy nowo utwożony
		}

		// kontruktor domyślny
		ShaderEngine(){};

		//dodawanie nowych filtrów
		template<typename TShader>
		ShaderEngine& operator+= (const TShader& shader)
		{
			procesors.push_back( new typename TShader::TShaderProcesor(shader) ); //dodaj to tablicy nowo utwożony
			return *this;
		}


		// destruktor
		~ShaderEngine()
		{
			for(TProcesors::iterator it = procesors.begin(); it != procesors.end(); it++) // dla każdego filtru
				delete *it;  // usuń go
		}

		boost::shared_ptr<BITMAP> operator()(boost::shared_ptr<BITMAP> inbitmap, boost::shared_ptr<BITMAP>& outbitmap) const
		{
			if(!inbitmap) throw "Niepoprawana bitmapa wejsciowa.";  // sprawdz bitmapę wejściową
			if(!outbitmap)		// sprawdz bitmapę wyjściową
				outbitmap = boost::shared_ptr<BITMAP>(create_bitmap_ex(32,inbitmap->w,inbitmap->h),&destroy_bitmap); // w razie potrzeby utwóż nową
			//else if(bitmap_color_depth(outbitmap.get())!=32) throw "32 bit ony!";
			acquire_bitmap( inbitmap.get() );
			acquire_bitmap( outbitmap.get() );
			for(TProcesors::const_iterator it = procesors.begin(); it != procesors.end(); it++)  // dla każdego filtru
				(*it)->run( inbitmap.get(), outbitmap.get() ); // uruchom go
			release_bitmap( inbitmap.get());
			release_bitmap( outbitmap.get());
			return outbitmap; //zwróć bitmapę wyjściową
		}

	private:
		TProcesors procesors;

};



#endif // SHADERENGINE_H
