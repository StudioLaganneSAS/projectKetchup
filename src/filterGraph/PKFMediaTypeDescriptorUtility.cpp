//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKFMediaTypeDescriptorUtility.h"

//
// PKFMediaTypeDescriptorUtility
//

bool PKFMediaTypeDescriptorUtility::matchMediaTypes(IPKFMediaTypeDescriptor *t1, IPKFMediaTypeDescriptor *t2)
{
	if(t1 == NULL || t2 == NULL)
	{
		return false;
	}

	switch(t1->getMediaType())
	{
	case PKF_MEDIA_TYPE_IMAGE:
		{
			switch(t2->getMediaType())
			{
			case PKF_MEDIA_TYPE_IMAGE:
				{
					PKFImageMediaTypeDescriptor *im1 = (PKFImageMediaTypeDescriptor *) t1;
					PKFImageMediaTypeDescriptor *im2 = (PKFImageMediaTypeDescriptor *) t2;

					if(im1->getNumberOfChannels() == 0 ||
					   im2->getNumberOfChannels() == 0)
					{
						// at least one connector
						// accepts all the types

						return true;
					}

					if(im1->getNumberOfChannels() != im2->getNumberOfChannels())
					{
						return false;
					}

					//
					// check channel types
					// 

					std::vector <uint32_t> channels;

					for(unsigned int i=0; i < im1->getNumberOfChannels(); i++)
					{
						PKFImageChannelDescriptor *ch1 = NULL;
						PKFReturnCode result = im1->getChannelDescriptor(i, &ch1);

						if(result != PKF_SUCCESS)
						{
							return false;
						}

						// find match

						bool found = false;

						for(unsigned int j=0; j < im2->getNumberOfChannels(); j++)
						{
							bool skip = false;

							for(unsigned int k=0; k < channels.size(); k++)
							{
								if(j == channels[k])
								{
									skip = true;
									break;
								}
							}

							if(skip)
							{
								continue;
							}

							PKFImageChannelDescriptor *ch2 = NULL;
							PKFReturnCode result = im2->getChannelDescriptor(j, &ch2);

							if(result != PKF_SUCCESS)
							{
								return false;
							}

							if( (ch1->getDescription() == ch2->getDescription()) &&
								(ch1->getImageDataType() == ch2->getImageDataType()) )
							{
								found = true;
								channels.push_back(j);
								break;
							}
						}

						if(!found)
						{
							return false;
						}
					}

					return true;
				}
				break;

			default:
				{
					return false;
				}
				break;
			}
		}
		break;

	case PKF_MEDIA_TYPE_DATA:
		{
			switch(t2->getMediaType())
			{
			case PKF_MEDIA_TYPE_DATA:
				{
					return true;
				}
				break;

			default:
				{
					return false;
				}
				break;
			}
		}
		break;

	case PKF_MEDIA_TYPE_CUSTOM:
		{
			switch(t2->getMediaType())
			{
			case PKF_MEDIA_TYPE_CUSTOM:
				{
					PKFCustomMediaTypeDescriptor *tt1 = (PKFCustomMediaTypeDescriptor *) t1;
					PKFCustomMediaTypeDescriptor *tt2 = (PKFCustomMediaTypeDescriptor *) t2;

					PKVariant *d1 = tt1->get(PKFCustomMediaTypeDescriptor::TYPE_IDENTIFIER_PROPERTY);
					PKVariant *d2 = tt2->get(PKFCustomMediaTypeDescriptor::TYPE_IDENTIFIER_PROPERTY);

					if(d1 == NULL || d2 == NULL)
					{
						return false;
					}

					if(PKVALUE_STRING(d1) == "" || PKVALUE_STRING(d2) == "")
					{
						// at least one pin accepts all 
						// the custom media types...

						return true;
					}

					return (PKVALUE_STRING(d1) == PKVALUE_STRING(d2));
				}
				break;

			default:
				{
					return false;
				}
				break;
			}
		}
		break;
	}

	return false;
}

IPKFMediaTypeDescriptor *PKFMediaTypeDescriptorUtility::create8BitARGBImageMediaType(uint32_t width, uint32_t height)
{
	PKFImageMediaTypeDescriptor *desc = new PKFImageMediaTypeDescriptor();

	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_ALPHA,     width, height, PKF_IMAGE_DATA_TYPE_8_BITS);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_RED,   width, height, PKF_IMAGE_DATA_TYPE_8_BITS);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_GREEN, width, height, PKF_IMAGE_DATA_TYPE_8_BITS);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_BLUE,  width, height, PKF_IMAGE_DATA_TYPE_8_BITS);

	return desc;
}

IPKFMediaTypeDescriptor *PKFMediaTypeDescriptorUtility::create8BitRGBImageMediaType(uint32_t width, uint32_t height)
{
	PKFImageMediaTypeDescriptor *desc = new PKFImageMediaTypeDescriptor();

	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_RED,   width, height, PKF_IMAGE_DATA_TYPE_8_BITS);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_GREEN, width, height, PKF_IMAGE_DATA_TYPE_8_BITS);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_BLUE,  width, height, PKF_IMAGE_DATA_TYPE_8_BITS);

	return desc;
}

IPKFMediaTypeDescriptor *PKFMediaTypeDescriptorUtility::create8BitGrayscaleImageMediaType(uint32_t width, uint32_t height)
{
	PKFImageMediaTypeDescriptor *desc = new PKFImageMediaTypeDescriptor();

	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_GRAYSCALE, width, height, PKF_IMAGE_DATA_TYPE_8_BITS);

	return desc;
}

IPKFMediaTypeDescriptor *PKFMediaTypeDescriptorUtility::create16BitARGBImageMediaType(uint32_t width, uint32_t height)
{
	PKFImageMediaTypeDescriptor *desc = new PKFImageMediaTypeDescriptor();

	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_ALPHA,     width, height, PKF_IMAGE_DATA_TYPE_16_BITS);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_RED,   width, height, PKF_IMAGE_DATA_TYPE_16_BITS);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_GREEN, width, height, PKF_IMAGE_DATA_TYPE_16_BITS);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_BLUE,  width, height, PKF_IMAGE_DATA_TYPE_16_BITS);

	return desc;
}

IPKFMediaTypeDescriptor *PKFMediaTypeDescriptorUtility::create16BitRGBImageMediaType(uint32_t width, uint32_t height)
{
	PKFImageMediaTypeDescriptor *desc = new PKFImageMediaTypeDescriptor();

	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_RED,   width, height, PKF_IMAGE_DATA_TYPE_16_BITS);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_GREEN, width, height, PKF_IMAGE_DATA_TYPE_16_BITS);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_BLUE,  width, height, PKF_IMAGE_DATA_TYPE_16_BITS);

	return desc;
}

IPKFMediaTypeDescriptor *PKFMediaTypeDescriptorUtility::create16BitGrayscaleImageMediaType(uint32_t width, uint32_t height)
{
	PKFImageMediaTypeDescriptor *desc = new PKFImageMediaTypeDescriptor();

	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_GRAYSCALE, width, height, PKF_IMAGE_DATA_TYPE_16_BITS);

	return desc;
}

IPKFMediaTypeDescriptor *PKFMediaTypeDescriptorUtility::createFloatARGBImageMediaType(uint32_t width, uint32_t height)
{
	PKFImageMediaTypeDescriptor *desc = new PKFImageMediaTypeDescriptor();

	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_ALPHA,     width, height, PKF_IMAGE_DATA_TYPE_FLOAT);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_RED,   width, height, PKF_IMAGE_DATA_TYPE_FLOAT);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_GREEN, width, height, PKF_IMAGE_DATA_TYPE_FLOAT);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_BLUE,  width, height, PKF_IMAGE_DATA_TYPE_FLOAT);

	return desc;
}

IPKFMediaTypeDescriptor *PKFMediaTypeDescriptorUtility::createFloatRGBImageMediaType(uint32_t width, uint32_t height)
{
	PKFImageMediaTypeDescriptor *desc = new PKFImageMediaTypeDescriptor();

	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_RED,   width, height, PKF_IMAGE_DATA_TYPE_FLOAT);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_GREEN, width, height, PKF_IMAGE_DATA_TYPE_FLOAT);
	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_RGB_BLUE,  width, height, PKF_IMAGE_DATA_TYPE_FLOAT);

	return desc;
}

IPKFMediaTypeDescriptor *PKFMediaTypeDescriptorUtility::createFloatGrayscaleImageMediaType(uint32_t width, uint32_t height)
{
	PKFImageMediaTypeDescriptor *desc = new PKFImageMediaTypeDescriptor();

	desc->addChannelDescriptor(PKFImageChannel::PKF_TYPE_GRAYSCALE, width, 0, PKF_IMAGE_DATA_TYPE_FLOAT);

	return desc;
}
