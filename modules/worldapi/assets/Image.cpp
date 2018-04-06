#include "Image.h"

#include <memory>

#ifdef WORLD_BUILD_OPENCV_MODULES
#include "Interop.h"

#include <stdexcept>
#include <opencv/cv.h>

#include "math/MathsHelper.h"

namespace world {

	inline int getCVType(const ImageType &type) {
		switch (type) {
		case ImageType::RGB :
			return CV_8UC3;
		case ImageType::RGBA :
			return CV_8UC4;
		default :
			return CV_8UC1;
		}
	}
	
	inline ImageType getImageType(int cvType) {
		switch (cvType) {
		case CV_8UC1 :
			return ImageType::GREYSCALE;
		case CV_8UC3:
			return ImageType::RGB;
		case CV_8UC4 :
			return ImageType::RGBA;
		default:
			throw std::runtime_error("Unknown type");
		}
	}

	inline u8 fromDouble(double f) {
		return clamp(static_cast<u8>(f * 255.0), (u8)0, (u8)255);
	}

	inline double toDouble(u8 u) {
		return u / 255.0;
	}

	class PrivateImage {
	public:
		PrivateImage(cv::Mat && image) : _image(image) {}

		cv::Mat _image;
	};

	// ImageStream implementation

	ImageStream::ImageStream(const world::Image &image)
			: _image(image) {

	}

	int ImageStream::remaining() {
		auto &mat = _image._private->_image;
		// size (in bytes) - position
		return static_cast<int>(mat.total() * mat.elemSize()) - _position;
	}

	int ImageStream::read(char *buffer, int count) {
		auto &mat = _image._private->_image;
		int read = 0;
		const int s = static_cast<int>(mat.elemSize());

		while (count >= s) {
			u8* data = mat.data + _position;

			switch (_image._type) {
				case ImageType::RGBA:
					// BGRA to ARGB
					buffer[read] = data[3]; read++;
				case ImageType::RGB:
					buffer[read] = data[2]; read++;
					buffer[read] = data[1]; read++;
				case ImageType::GREYSCALE:
					buffer[read] = data[0]; read++;
			}

			count -= s;
			_position += s;
		}

		return _position;
	}

	// Implémentation de Pixel

	ConstPixel::ConstPixel(int x, int y, Image * ref)
		: _x(x), _y(y), _ref(ref) {

	}

	Pixel::Pixel(int x, int y, Image * ref) : ConstPixel(x, y, ref) {

	}

	void Pixel::set(u8 r, u8 g, u8 b, u8 a) {
		switch (_ref->_type) {
		case ImageType::RGB :
			_ref->_private->_image.at<cv::Vec3b>(_y, _x) = cv::Vec3b(b, g, r);
			break;
		case ImageType::RGBA :
			_ref->_private->_image.at<cv::Vec4b>(_y, _x) = cv::Vec4b(b, g, r, a);
			break;
		default :
			_ref->_private->_image.at<u8>(_y, _x) = r;
		}
	}

	void Pixel::setf(double r, double g, double b, double a) {
		set(fromDouble(r), fromDouble(g), fromDouble(b), fromDouble(a));
	}

	void Pixel::setRed(u8 r) {
		setComponent(2, r);
	}

	void Pixel::setGreen(u8 g) {
		setComponent(1, g);
	}

	void Pixel::setBlue(u8 b) {
		setComponent(0, b);
	}

	void Pixel::setAlpha(u8 a) {
		setComponent(3, a);
	}

	void Pixel::setRedf(double r) {
		setComponent(2, fromDouble(r));
	}

	void Pixel::setGreenf(double r) {
		setComponent(1, fromDouble(r));
	}

	void Pixel::setBluef(double r) {
		setComponent(0, fromDouble(r));
	}

	void Pixel::setAlphaf(double r) {
		setComponent(3, fromDouble(r));
	}

	void Pixel::setLevel(u8 l) {
		setComponent(0, l);
	}

	void Pixel::setLevelf(double l) {
		setComponent(0, fromDouble(l));
	}

	u8 ConstPixel::getAlpha() const {
		return getComponent(3);
	}

	u8 ConstPixel::getRed() const {
		return getComponent(2);
	}

	u8 ConstPixel::getGreen() const {
		return getComponent(1);
	}

	u8 ConstPixel::getBlue() const {
		return getComponent(0);
	}

	double ConstPixel::getRedf() const {
		return toDouble(getComponent(2));
	}

	double ConstPixel::getGreenf() const {
		return toDouble(getComponent(1));
	}

	double ConstPixel::getBluef() const {
		return toDouble(getComponent(0));
	}

	double ConstPixel::getAlphaf() const {
		return toDouble(getComponent(3));
	}

	u8 ConstPixel::getComponent(int id) const {
		switch (_ref->_type) {
		case ImageType::RGBA:
			return _ref->_private->_image.data[4 * (_ref->_private->_image.cols * _y + _x) + id];
		case ImageType::RGB:
			if (id == 3) return 255;
			return _ref->_private->_image.data[3 * (_ref->_private->_image.cols * _y + _x) + id];
		default:
			if (id == 3) return 255;
			return _ref->_private->_image.data[_ref->_private->_image.cols * _y + _x];
		}
	}

	void Pixel::setComponent(int id, u8 value) {
		switch (_ref->_type) {
		case ImageType::RGB:
			if (id == 3) return;
			_ref->_private->_image.data[3 * (_ref->_private->_image.cols * _y + _x) + id] = value;
			break;
		case ImageType::RGBA:
			_ref->_private->_image.data[4 * (_ref->_private->_image.cols * _y + _x) + id] = value;
			break;
		default:
			if (id == 3) return;
			_ref->_private->_image.data[_ref->_private->_image.cols * _y + _x] = value;
		}
	}



	// Implémentation de Image

	Image::Image(int width, int height, const ImageType &type)
		: _private(new PrivateImage(cv::Mat(height, width, getCVType(type)))),
		_type(type) {

	}

	Image::Image(const arma::Cube<double> & data) {
		_private = new PrivateImage(cv::Mat(armaToCV(data)));
		_type = getImageType(_private->_image.type());
	}

	Image::Image(const arma::Mat<double> & data) {
		_private = new PrivateImage(cv::Mat(armaToCV(data)));
		_type = getImageType(_private->_image.type());
	}

	Image::Image(const std::string & filename) {
        cv::Mat mat = cv::imread(filename);

        if (mat.data == nullptr) {
            throw std::ios_base::failure("File not found : " + filename);
        }

		_private = new PrivateImage(std::move(mat));
		_type = getImageType(_private->_image.type());
	}

	Image::Image(const char *filename) : Image(std::string(filename)) {

	}

	Image::Image(Image && image) : _private(image._private), _type(image._type) {
		image._private = nullptr;
	}

	Image::Image(const Image & image) {
		_private = new PrivateImage(cv::Mat(image._private->_image));
		_type = image._type;
	}

	Image::~Image() {
		if (_private != nullptr) // moved
			delete _private;
	}

	ImageType Image::type() const {
		return _type;
	}

	int Image::width() const {
		return _private->_image.cols;
	}

	int Image::height() const {
		return _private->_image.rows;
	}

	Pixel Image::at(int x, int y) {
		return Pixel(x, y, this);
	}

	const ConstPixel Image::at(int x, int y) const {
		return ConstPixel(x, y, const_cast<Image *>(this));
	}

	void Image::write(const std::string &file) const {
		cv::imwrite(file, _private->_image);
	}
}

#else // WORLD_BUILD_OPENCV_MODULES

#include <libpng/png.h>

namespace world {

	inline u8 fromDouble(double f) {
		return f >= 1.0 ? (u8) 255 : (f <= 0.0 ? (u8) 0 : (u8) (f * 256.0));
	}

	inline double toDouble(u8 u) {
		return u / 255.0;
	}

	class PrivateImage {
	public:
		/*PrivateImage(png_structp img) : _image(img) {}

		png_structp _image;*/
	};


	ConstPixel::ConstPixel(int x, int y, Image * ref)
		: _x(x), _y(y), _ref(ref) {

	}

	Pixel::Pixel(int x, int y, Image * ref) : ConstPixel(x, y, ref) {

	}

	void Pixel::set(u8 r, u8 g, u8 b, u8 a) {

	}

	void Pixel::setf(double r, double g, double b, double a) {
		set(fromDouble(r), fromDouble(g), fromDouble(b), fromDouble(a));
	}

	void Pixel::setRed(u8 r) {
		setComponent(2, r);
	}

	void Pixel::setGreen(u8 g) {
		setComponent(1, g);
	}

	void Pixel::setBlue(u8 b) {
		setComponent(0, b);
	}

	void Pixel::setAlpha(u8 a) {
		setComponent(3, a);
	}

	void Pixel::setRedf(double r) {
		setComponent(2, fromDouble(r));
	}

	void Pixel::setGreenf(double r) {
		setComponent(1, fromDouble(r));
	}

	void Pixel::setBluef(double r) {
		setComponent(0, fromDouble(r));
	}

	void Pixel::setAlphaf(double r) {
		setComponent(3, fromDouble(r));
	}

	void Pixel::setLevel(u8 l) {
		setComponent(0, l);
	}

	void Pixel::setLevelf(double l) {
		setComponent(0, fromDouble(l));
	}

	u8 ConstPixel::getAlpha() const {
		return getComponent(3);
	}

	u8 ConstPixel::getRed() const {
		return getComponent(2);
	}

	u8 ConstPixel::getGreen() const {
		return getComponent(1);
	}

	u8 ConstPixel::getBlue() const {
		return getComponent(0);
	}

	double ConstPixel::getRedf() const {
		return toDouble(getComponent(2));
	}

	double ConstPixel::getGreenf() const {
		return toDouble(getComponent(1));
	}

	double ConstPixel::getBluef() const {
		return toDouble(getComponent(0));
	}

	double ConstPixel::getAlphaf() const {
		return toDouble(getComponent(3));
	}

	u8 ConstPixel::getComponent(int id) const {
		return 0;
	}

	void Pixel::setComponent(int id, u8 value) {

	}



	// Implémentation de Image

	Image::Image(int width, int height, const ImageType &type)
		: _private(),
		_type(type) {

	}

	Image::Image(const arma::Cube<double> & data) {
		_private = new PrivateImage();
	}

	Image::Image(const arma::Mat<double> & data) {
		_private = new PrivateImage();
	}

	Image::Image(const std::string & filename) {
		_private = new PrivateImage();
	}

	Image::Image(const char *filename) : Image(std::string(filename)) {

	}

	Image::Image(Image && image) : _private(image._private), _type(image._type) {
		image._private = nullptr;
	}

	Image::Image(const Image & image) {
		_private = new PrivateImage();
		_type = image._type;
	}

	Image::~Image() {
		if (_private != nullptr) // moved
			delete _private;
	}

	ImageType Image::type() const {
		return _type;
	}

	int Image::width() const {
		return 0;
	}

	int Image::height() const {
		return 0;
	}

	Pixel Image::at(int x, int y) {
		return Pixel(x, y, this);
	}

	const ConstPixel Image::at(int x, int y) const {
		return ConstPixel(x, y, const_cast<Image *>(this));
	}

	void Image::write(const std::string &file) const {

	}
}

#endif // WORLD_BUILD_OPENCV_MODULES