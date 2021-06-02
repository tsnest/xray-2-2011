//   gaussianblur.h - declaration of template class
//   of 1D and 2D gaussian blur or filter
//
//   The code is property of LIBROW
//   You can use it on your own
//   When utilizing credit LIBROW site

#ifndef _GAUSSIANBLUR_H_
#define _GAUSSIANBLUR_H_

template <class T = double> class TGaussianBlur
{
public:
	//   1D GAUSSIAN BLUR
	//     pSignal - input signal;
	//     pResult - output signal, NULL for inplace processing
	//     N       - length of the signal
	//     W       - window size, odd positive number
	bool Filter(T *pSignal, T *pResult, unsigned int N, unsigned int W) const;

	//   2D GAUSSIAN BLUR
	//     pImage  - input image
	//     pResult - output image, NULL for inplace processing
	//     N       - width of the image
	//     M       - height of the image
	//     W       - window size, odd positive number
	bool Filter(T *pImage, T *pResult, unsigned int N, unsigned int M, unsigned int W) const;

protected:
	//   Internal auxiliary structure - data array size descriptor
	struct CSize
	{
		unsigned int x;   //   Array width
		unsigned int y;   //   Array height

		//   Default constructor
		CSize(): x(0), y(0) {}
		//   Constructor with initialization
		CSize(unsigned int _x, unsigned int _y): x(_x), y(_y) {}

		//   Initialization
		void Set(unsigned int _x, unsigned int _y) { x = _x; y = _y; }
		//   Area
		unsigned int Area() const { return x * y; }
	};

	//   Internal auxiliary structure - array descriptor
	struct CArray
	{
		CSize Size;   //   Array size
		T *Buffer;    //   Element buffer

		//   Default constructor
		CArray(): Buffer(NULL) {}
		//   Constructors with initialization
		CArray(T *_Buffer, const CSize &_Size): Buffer(_Buffer), Size(_Size) {}
		CArray(T *_Buffer, unsigned int _N): Buffer(_Buffer), Size(_N, 1) {}
	};

	//   Internal auxiliary structure - array extension descriptor
	struct CExtension: public CArray
	{
		unsigned int Margin;   //   Extension margins

		enum EMode {ModeHorizontal, ModeVertical};

		//   Default cosntructor
		CExtension(): Margin(0), Mode(ModeHorizontal) {}
		//   Destructor
		~CExtension() { if (Buffer) FREE(Buffer); }

		//   Mode setting
		void SetMode(EMode _Mode) { Mode = _Mode; }
		//   Extension memory allocation
		bool Allocate(unsigned int _N, unsigned int _W) { return _Allocate(CSize(_N, 1), _W >> 1); }
		bool Allocate(const CSize &_Size, unsigned int _W) { return _Allocate(_Size, _W >> 1); }
		//   Pasting data into extension from data array
		void Paste(const T * _Start);
		//   Extension
		void Extend();

	protected:
		EMode Mode;   //   Current mode

		//   Extension memory allocation
		bool _Allocate(const CSize &_Size, unsigned int _Margin);
	};

	//   Internal auxiliary structure - filter window descriptor
	struct CWindow : public boost::noncopyable
	{
		double *Weights;   //   Window weights
		unsigned int Size;   //   Window size

		//   Default constructor
		CWindow(): Weights(NULL), Size(0), Correction(.5 - double(T(.5))) {}
		//   Destructor
		~CWindow() { if (Weights) FREE(Weights); }

		//   Filter window creation
		bool Create(unsigned int _Size);

		//   FILTER WINDOW APPLICATION
		//     _Element - start element in signal/image
		T Apply(const T *_Element) const
		{
			//   Apply filter - calculate weighted mean
			double Sum = 0.;
			const double *WeightIter = Weights;
			const T *ElIter = _Element;
			const double *const End = Weights + Size;
			while (WeightIter < End)
				Sum += *(WeightIter++) * double(*(ElIter++));
			return T(Sum + Correction);
		}

	protected:
		const double Correction;
	};

	//   Internal auxiliary functions - check input data consistency
	bool Consistent(const T *_Image, const CSize &_Size, unsigned int _W) const { return  _Image && _Size.x && _Size.y && _W && _Size.x > (_W >> 1) && _Size.y > (_W >> 1) && _W & 1; }
	bool Consistent(const T *_Signal, unsigned int _N, unsigned int _W) const { return  _Signal && _N && _W && _N > (_W >> 1) && _W & 1; }
};
#include "gaussianblur_inline.h"
#endif

