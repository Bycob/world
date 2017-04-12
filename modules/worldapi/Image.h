#pragma once
#include "worldapidef.h"

#include <armadillo/armadillo>
#include <stdint.h>

namespace cv {
	class Mat;
}

namespace img {
	class Image;

	enum class WORLDAPI_EXPORT ImageType {
		RGB = 0,
		RGBA = 1,
		GREYSCALE = 2
	};

	class WORLDAPI_EXPORT Pixel {
	public:
		void set(uint8_t r, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255);
		void setf(float r, float g = 0, float b = 0, float a = 1);

		void setAlpha(uint8_t a);
		void setRed(uint8_t r);
		void setGreen(uint8_t g);
		void setBlue(uint8_t b);

		void setAlphaf(float a);
		void setRedf(float r);
		void setGreenf(float g);
		void setBluef(float b);

		void setLevel(uint8_t l);
		void setLevelf(float l);

		uint8_t getAlpha() const;
		uint8_t getRed() const;
		uint8_t getGreen() const;
		uint8_t getBlue() const;

	private:
		friend class Image;

		Pixel(int width, int height, Image *ref);

		uint8_t getComponent(int id) const;
		void setComponent(int id, uint8_t type);

		int _x; int _y;
		Image * _ref;
	};

	class WORLDAPI_EXPORT Image {
	public:
		Image(int width, int height, const ImageType &type);
		Image(const arma::Cube<double> & data);
		Image(const arma::Mat<double> & data);
		Image(const std::string & filename);
		// move constructor
		Image(Image && img);
		// copy constructor
		Image(const Image & img);
		~Image();

		// infos
		ImageType type() const;
		uint32_t width() const;
		uint32_t height() const;

		// access
		Pixel at(int x, int y);
		const Pixel at(int x, int y) const;

		// IO
		void write(const std::string &file);
	private:
		ImageType _type;

		// pointeur sur l'image, pour éviter d'avoir à inclure opencv dans le .h
		cv::Mat *_image;
		
		friend class Pixel;
	};
}

