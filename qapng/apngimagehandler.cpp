#include "apngimagehandler.h"
#include <QFileDevice>
#include <io.h>
#include <loadapng.h>

void apngCleanupHandler(void *info);

ApngImageHandler::ApngImageHandler() :
	QImageIOHandler(),
	currentIndex(0),
	imageCache(),
	readState(false)
{}

QByteArray ApngImageHandler::name() const
{
	return "apng";
}

bool ApngImageHandler::canRead() const
{
	if(this->readState)
		return !this->imageCache.isEmpty();
	else
		return this->device();
}

bool ApngImageHandler::read(QImage *image)
{
	auto data = this->getData();
	*image = data[this->currentIndex].first;
	if(++this->currentIndex >= data.size())
		this->currentIndex = 0;
	return !image->isNull();
}

QVariant ApngImageHandler::option(QImageIOHandler::ImageOption option) const
{
	switch(option) {
	case QImageIOHandler::IncrementalReading:
	case QImageIOHandler::Animation:
		return true;
	default:
		return QVariant();
	}
}

bool ApngImageHandler::supportsOption(QImageIOHandler::ImageOption option) const
{
	switch(option) {
	case QImageIOHandler::IncrementalReading:
	case QImageIOHandler::Animation:
		return true;
	default:
		return false;
	}
}

bool ApngImageHandler::jumpToNextImage()
{
	if(this->currentIndex < this->getData().size() - 1) {
		++this->currentIndex;
		return true;
	} else
		return false;
}

bool ApngImageHandler::jumpToImage(int imageNumber)
{
	if(imageNumber < this->getData().size() - 1) {
		this->currentIndex = imageNumber;
		return true;
	} else
		return false;
}

int ApngImageHandler::loopCount() const
{
	return -1;
}

int ApngImageHandler::imageCount() const
{
	return this->imageCache.size();
}

int ApngImageHandler::nextImageDelay() const
{
	return this->imageCache[this->currentIndex].second;
}

int ApngImageHandler::currentImageNumber() const
{
	return this->currentIndex;
}

QVector<ApngImageHandler::ImageInfo> &ApngImageHandler::getData()
{
	if(!this->readState) {
		this->readImageData();
		this->readState = true;
	}
	return this->imageCache;
}

bool ApngImageHandler::readImageData()
{
	auto device = dynamic_cast<QFileDevice*>(this->device());
	auto handle = device->handle();
	auto fHandle = fdopen(_dup(handle), "rb");
	if(fHandle != 0) {
		std::vector<APNGFrame> frames;
		auto res = load_apng(fHandle, frames);
		fclose(fHandle);

		if(res >= 0) {
			this->imageCache = QVector<ImageInfo>((int)frames.size());
			for(int i = 0, max = frames.size(); i < max; ++i) {
				APNGFrame &frame = frames[i];
				QImage image(frame.p, frame.w, frame.h, QImage::Format_RGBA8888, apngCleanupHandler, new APNGFrame(frame));
				this->imageCache[i] = {image, qRound(((double)frame.delay_num / (double)frame.delay_den) * 1000.0)};
			}
			return true;
		}
	}

	return false;
}

void apngCleanupHandler(void *info)
{
	auto frame = static_cast<APNGFrame*>(info);
	delete[] frame->rows;
	delete[] frame->p;
	delete frame;
}
