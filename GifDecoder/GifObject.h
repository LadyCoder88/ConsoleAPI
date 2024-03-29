#pragma once
#include "Header.h"
#include <vector>
#include "GraphicControlExtension.h"

namespace Gif
{
    struct DataStream;

    struct RGBAColor
    {
        RGBAColor() {}
        RGBAColor(Color RGBColor, bool isTransparent)
            :m_RGBColor(RGBColor),
            m_alpha(isTransparent ? 0 : 255) {}

        unsigned char m_alpha = 255;
        Color m_RGBColor;
    };

    class GifObject
    {
        friend class GifDecoder;
        
    private:
        GifObject(const DataStream& data_stream);

        void CopyData(const GifObject& gif_object);
        void MoveData(GifObject&& gif_object);
        
    public:
        class Frame;

        GifObject(const GifObject& gif_object);
        GifObject(GifObject&& gif_object);
        
        GifObject& operator=(const GifObject& gif_object);
        GifObject& operator=(GifObject&& gif_object);

        ~GifObject();

        int GetFrameCount() const { return m_framesCount; }
        short GetImageHeight() const { return m_imageHeight; }
        short GetImageWigth() const { return m_imageWidth; }

        //Function can throw std::out_of_range exception
        const Frame& GetFrame(int idx) const;

        const std::string& GetFileName() const { return m_fileName; }
        const RGBAColor& GetBackgroundColor() const { return m_backgroundColor; }

        class FrameInfo
        {
            friend class GifObject;

        public:
            short GetFrameSizeX() const { return m_sizeX; }
            short GetFrameSizeY() const { return m_sizeY; }
            short GetFramePositionX() const { return m_coordX; }
            short GetFramePositionY() const { return m_coordY; }
            short GetDelayTime() const { return m_delay; }
            const Gif::GraphicControlExtension::EDisposeMethod GetDisposeMethod() const { return m_disposeMethod; }

        protected:
            FrameInfo(short size_x, short size_y)
                :m_sizeX(size_x), m_sizeY(size_y), m_coordX(0), m_coordY(0) {}
            FrameInfo(short size_x, short size_y, short coord_x, short coord_y, short _delay, Gif::GraphicControlExtension::EDisposeMethod disposeMethod)
                :m_sizeX(size_x), m_sizeY(size_y), m_coordX(coord_x), m_coordY(coord_y), m_delay(_delay), m_disposeMethod(disposeMethod) {}
            FrameInfo()
                :m_sizeX(0), m_sizeY(0), m_coordX(0), m_coordY(0) {}

            short m_sizeX = 0;
            short m_sizeY = 0;
            short m_coordX = 0;
            short m_coordY = 0;
            short m_delay = 0;

            Gif::GraphicControlExtension::EDisposeMethod m_disposeMethod;
        };

        class Frame
        {
            friend class GifObject;

        public:
            ~Frame();
            
            int GetDataSize() const { return m_dataSize; }
            const RGBAColor* const GetData() const { return m_data; }
            void SetColor(int idx, RGBAColor color);
            const FrameInfo& GetFrameInfo() const { return m_frameInfo; }

        protected:
            Frame(const FrameInfo& frame_info);
            Frame(const Frame& frame);
            Frame(Frame&& frame);

            FrameInfo m_frameInfo;
            unsigned int m_dataSize;
            RGBAColor* m_data;
        };

    private:
        std::string m_fileName;

        unsigned int m_framesCount;

        std::vector<Frame*> m_frames;

        RGBAColor m_backgroundColor;
        short m_imageHeight;
        short m_imageWidth;
    };

};