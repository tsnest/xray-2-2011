//   gaussianblur.cpp - impelementation of template class
//   of 1D and 2D gaussian blur or filter
//
//   The code is property of LIBROW
//   You can use it on your own
//   When utilizing credit LIBROW site

//   Include declaration file
//   Include math library
//#include <math.h>

//   1D GAUSSIAN BLUR
//     pSignal - input signal;
//     pResult - output signal, NULL for inplace processing
//     N       - length of the signal
//     W       - window size, odd positive number
template <class T> bool TGaussianBlur<T>::Filter(T *pSignal, T *pResult, unsigned int N, unsigned int W) const
{
	//   Check input data cosnsitency
	if (!Consistent(pSignal, N, W))
		return false;
	//   Allocate extension
	CExtension Extension;
	if (!Extension.Allocate(N, W))
		return false;
	//   Create signal descriptor
	const CArray Signal(pSignal, N);
	//   Paste signal into extension
	Extension.Paste(Signal.Buffer);
	//   Extend signal
	Extension.Extend();
	//   Create filter window
	CWindow Window;
	if (!Window.Create(W))
		return false;
	//   Extension iterator
	const T *ExtIter = Extension.Buffer;
	//   Extension stop position
	const T *const ExtStop = Extension.Buffer + Extension.Size.x;
	//   Result iterator
	T *ResIter = pResult ? pResult : pSignal;
	//   Filter - apply to every element
	while (ExtIter < ExtStop)
		*(ResIter++) = Window.Apply(ExtIter++);
	//   Succeeded
	return true;
}

//   2D GAUSSIAN BLUR
//     pImage  - input image
//     pResult - output image, NULL for inplace processing
//     N       - width of the image
//     M       - height of the image
//     W       - window size
template <class T> bool TGaussianBlur<T>::Filter(T *pImage, T *pResult, unsigned int N, unsigned int M, unsigned int W) const
{
	//   Check input data consistency
	if (!Consistent(pImage, CSize(N, M), W))
		return false;
	//   Allocate extension
	CExtension Extension;
	if (!Extension.Allocate(CSize(N, M), W))
		return false;
	//   Create image descriptor
	CArray Image(pImage, CSize(N, M));
	//   Create filter window
	CWindow Window;
	if (!Window.Create(W))
		return false;
	//   Stop postion
	const T * ExtStop = Extension.Buffer + Extension.Size.x;
	//   Result iterator
	T *ResIter = pResult ? pResult : pImage;
	//   Image iterator
	const T *ImIter = Image.Buffer;
	//   Image stop position
	const T * ImStop = Image.Buffer + Image.Size.Area();
	//   Filter line by line
	while (ImIter < ImStop)
	{
		//   Paste image line into extension
		Extension.Paste(ImIter);
		//   Extend image line
		Extension.Extend();
		//   Extension iterator
		const T *ExtIter = Extension.Buffer;
		//   Apply filter to every pixel of the line
		while (ExtIter < ExtStop)
			*(ResIter++) = Window.Apply(ExtIter++);
		//   Move to the next line
		ImIter += Image.Size.x;
	}
	//   Initialize image descriptor with filter result
	Image.Buffer = pResult ? pResult : pImage;
	//   Set vertical extension mode
	Extension.SetMode(CExtension::ModeVertical);
	//   Extension stop position
	ExtStop = Extension.Buffer + Extension.Size.y;
	//   Result column iterator
	T *ResColumnIter = pResult ? pResult : pImage;
	//   Image iterator
	ImIter = Image.Buffer;
	//   Image stop position
	ImStop = Image.Buffer + Image.Size.x;
	//   Filter column by column
	while (ImIter < ImStop)
	{
		//   Paste image column into extension
		Extension.Paste(ImIter++);
		//   Extend image column
		Extension.Extend();
		//   Extension iterator
		const T *ExtIter = Extension.Buffer;
		//   Result pixel iterator
		ResIter = ResColumnIter;
		//   Apply fitler to every pixel of the column
		while (ExtIter < ExtStop)
		{
			*ResIter = Window.Apply(ExtIter++);
			ResIter += Image.Size.x;
		}
		//   Move to the next column
		++ResColumnIter;
	}
	//   Succeeded
	return true;
}

//   EXTENSION MEMORY ALLOCATION
//     _Size   - signal/image size
//     _Margin - extension margins
template <class T> bool TGaussianBlur<T>::CExtension::_Allocate(const CSize &_Size, unsigned int _Margin)
{
	//   Allocate extension buffer
//	Buffer = new T[(_Size.x > _Size.y ? _Size.x : _Size.y) + (_Margin << 1)];
	Buffer = ALLOC(  T, (_Size.x > _Size.y ? _Size.x : _Size.y) + (_Margin << 1) );
	//   Check buffer allocation
	if (!Buffer)
		//   Buffer allocation failed
		return false;
	//   Initialize size descriptors
	Size = _Size;
	Margin = _Margin;
	//   Succeeded
	return true;
}

//   PASTING DATA LINE/COLUMN INTO EXTENSION FROM DATA ARRAY
//     _Start - start postion in image/signal to paste from
template <class T> void TGaussianBlur<T>::CExtension::Paste(const T *const _Start)
{
	if (Mode == ModeHorizontal)
	{
		//   Paste line
		memcpy(Buffer + Margin, _Start, Size.x * sizeof(T));
	}
	else
	{
		//   Stop position
		const T *const Stop = _Start + Size.Area();
		//   Array iterator
		const T *ArrIter = _Start;
		//   Extension iterator
		T *ExtIter = Buffer + Margin;
		//   Paste array column element by element
		while (ArrIter < Stop)
		{
			//   Copy line
			*(ExtIter++) = *ArrIter;
			//   Jump to the next line
			ArrIter += Size.x;
		}
	}
}

//   EXTENSION CREATION
template <class T> void TGaussianBlur<T>::CExtension::Extend()
{
	//   Line size
	const unsigned int Line = Mode == ModeHorizontal ? Size.x : Size.y;
	//   Stop position
	const T *const Stop = Buffer - 1;
	//   Left extension iterator
	T *ExtLeft = Buffer + Margin - 1;
	//   Left array iterator
	const T *ArrLeft = ExtLeft + 2;
	//   Right extension iterator
	T *ExtRight = ExtLeft + Line + 1;
	//   Left array iterator
	const T *ArrRight = ExtRight - 2;
	//   Create extension line element by element
	while (ExtLeft > Stop)
	{
		//   Left extension
		*(ExtLeft--) = *(ArrLeft++);
		//   Right extension
		*(ExtRight++) = *(ArrRight--);
	}
}

//   FILTER WINDOW CREATION
//     _Size - window size
template <class T> bool TGaussianBlur<T>::CWindow::Create(unsigned int _Size)
{
	//   Allocate window buffer
	Weights = ALLOC( double, _Size);
	//   Check allocation
	if (!Weights)
		//   Window buffer allocation failed
		return false;
	//   Set window size
	Size = _Size;
	//   Window half
	const unsigned int Half = Size >> 1;
	//   Central weight
	Weights[Half] = 1.;
	//   The rest of weights
	for (unsigned int Weight = 1; Weight < Half + 1; ++Weight)
	{
		//   Support point
		const double x = 3.* double(Weight) / double(Half);
		//   Corresponding symmetric weights
		Weights[Half - Weight] = Weights[Half + Weight] = exp(-x * x / 2.);
	}
	//   Weight sum
	double k = 0.;
	for (unsigned int Weight = 0; Weight < Size; ++Weight)
		k += Weights[Weight];
	//   Weight scaling
	for (unsigned int Weight = 0; Weight < Size; ++Weight)
		Weights[Weight] /= k;
	//   Succeeded
	return true;
}