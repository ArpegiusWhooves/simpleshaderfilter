#ifndef SCANLINER_H
#define SCANLINER_H

#include "ShaderProcesor.hpp"

template<typename TShader>
class Scanliner : public ShaderProcesor   // procesor skanowania liniowego
{
	public:

		template<typename CShader>
		Scanliner(CShader _shader) : shader(_shader)  // zainitializuj filtr
		{

		}

		~Scanliner()
		{

		}

		// strony
		enum TDirection
		{	DOWN = 1
		,	RIGHT = 2
		,	UP = 4
		,	LEFT = 8
		};

		void run(BITMAP* in,BITMAP* out)
		{
			if(shader.dir & DOWN) process_downright(in,out); // proces przejścia z lewej do prawej
			if(shader.dir & RIGHT) process_topleft(in,out);  // proces przejścia z prawej do lewej
			if(shader.dir & UP) process_leftdown(in,out);    // proces przejścia z góry do dołu
			if(shader.dir & LEFT) process_righttop(in,out);  // proces przejścia z dołu do góry
		}

	protected:
		void process_downright(BITMAP* in,BITMAP* out);  // proces przejścia z lewej do prawej
		void process_topleft(BITMAP* in,BITMAP* out);    // proces przejścia z prawej do lewej
		void process_leftdown(BITMAP* in,BITMAP* out);   // proces przejścia z góry do dołu
		void process_righttop(BITMAP* in,BITMAP* out);   // proces przejścia z dołu do góry

	private:
		TShader shader;   // obiekt filtru
};

/** @brief process
  *
  * @todo: document this function
  */
template<typename TShader>
void Scanliner<TShader>::process_downright(BITMAP* in,BITMAP* out) // proces przejścia z lewej do prawej
{
	// initializacja argumentów
	typename TShader::TArgument *agregate = new typename TShader::TArgument[in->w + shader.backbuf];// initializacja argumentów
	agregate+=shader.backbuf-1; // cofnij o ilość argómentów do jakich shader się odwołuje wstecz
	typename TShader::TArgument *arg = agregate;  // dodatkowy wskaźnik na argumenty

	int mx	= in->w;   // zakres współżędnej x
	int my	= in->h;   // zakres współżędnej y

	for(int y = 0; y != my; ++y)  // dla każdej linijki po współżędnej y
	{
		arg = agregate; // zachowaj wskaźnik do początku listy argumentów
		for(int x = 0; x != mx; ++x) // dla każdej linijki po współżędnej x
		{
			int color = getpixel(out , x , y);// pobierz wartość pixla z wyjścia
			shader.eachpixel(++arg , getpixel(in , x , y) , color); // uruchom kod filtra, przejść do następnego arugmentu
			putpixel(out,x,y,color);// wstaw pixel na wyjście
		}
	}

	agregate-=shader.backbuf-1;  // przywróć orginalną pozcyje wksaźnika
	delete[] agregate;
}

/** @brief process
  *
  * @todo: document this function
  */
template<typename TShader>
void Scanliner<TShader>::process_topleft(BITMAP* in,BITMAP* out) // proces przejścia z prawej do lewej
{
	// initializacja argumentów
	typename TShader::TArgument *agregate = new typename TShader::TArgument[in->w + shader.backbuf]; // initializacja argumentów

	agregate+=shader.backbuf-1; // cofnij o ilość argómentów do jakich shader się odwołuje wstecz

	typename TShader::TArgument *arg = agregate;  // dodatkowy wskaźnik na argumenty

	int mx	= in->w - 1; // zakres współżędnej x
	int my	= in->h - 1; // zakres współżędnej y

	for(int y = my; y >= 0; --y)// dla każdej linijki po współżędnej y
	{
		arg = agregate;// zachowaj wskaźnik do początku listy argumentów
		for(int x = mx; x >= 0; --x)// dla każdej linijki po współżędnej x
		{
			int color = getpixel(out , x , y);  // pobierz wartość pixla z wyjścia
			shader.eachpixel(++arg, getpixel(in , x , y) , color); // uruchom kod filtra, przejść do następnego arugmentu
			putpixel(out,x,y,color);// wstaw pixel na wyjście
		}
	}
	agregate-=shader.backbuf-1;  // przywróć orginalną pozcyje wksaźnika
	delete[] agregate;
}


/** @brief process
  *
  * @todo: document this function
  */
template<typename TShader>
void Scanliner<TShader>::process_leftdown(BITMAP* in,BITMAP* out) // proces przejścia z góry do dołu
{
	typename TShader::TArgument *agregate = new typename TShader::TArgument[in->h + shader.backbuf]; // initializacja argumentów
	agregate+=shader.backbuf-1; // cofnij o ilość argómentów do jakich shader się odwołuje wstecz
	typename TShader::TArgument *arg = agregate;  // dodatkowy wskaźnik na argumenty

	int mx	= in->w - 1;  // zakres współżędnej x
	int my	= in->h; // zakres współżędnej y

	//std::cout << mx << " " << my << ":::";
	for(int x = mx; x >= 0; --x) // dla każdej linijki po współżędnej x
	{
		arg = agregate;// zachowaj wskaźnik do początku listy argumentów
		for(int y = 0; y != my; ++y)// dla każdej linijki po współżędnej y
		{
			int color = getpixel(out , x , y);// pobierz wartość pixla z wyjścia
			//std::cout << x << " " << y << " ;  ";
			shader.eachpixel(++arg , getpixel(in , x , y) , color); // uruchom kod filtra, przejść do następnego arugmentu
			putpixel(out,x,y,color);// wstaw pixel na wyjście
		}
	}

	agregate-=shader.backbuf-1; // przywróć orginalną pozcyje wksaźnika
	delete[] agregate;
}

/** @brief process
  *
  * @todo: document this function
  */
template<typename TShader>
void Scanliner<TShader>::process_righttop(BITMAP* in,BITMAP* out) // proces przejścia z dołu do góry
{
	typename TShader::TArgument *agregate = new typename TShader::TArgument[in->h + shader.backbuf]; // initializacja argumentów
	agregate+=shader.backbuf-1; // cofnij o ilość argómentów do jakich shader się odwołuje wstecz
	typename TShader::TArgument *arg = agregate;  // dodatkowy wskaźnik na argumenty

	int mx	= in->w;  // zakres współżędnej x
	int my	= in->h - 1; // zakres współżędnej y

	for(int x = 0; x != mx; ++x)// dla każdej linijki po współżędnej x
	{
		arg = agregate;// zachowaj wskaźnik do początku listy argumentów
		for(int y = my; y >= 0; --y)// dla każdej linijki po współżędnej y
		{
			int color = getpixel(out , x , y);// pobierz wartość pixla z wyjścia
			shader.eachpixel(++arg , getpixel(in , x , y) , color); // uruchom kod filtra, przejść do następnego arugmentu
			putpixel(out,x,y,color);  // wstaw pixel na wyjście
		}
	}

	agregate-=shader.backbuf-1; // przywróć orginalną pozcyje wksaźnika
	delete[] agregate;
}


#endif // SCANLINER_H
