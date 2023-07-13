# webp

基于Google libwebp解析webp文件播放动画

1.支持本地webp播放

2.支持网络webp链接播放

官方网站
https://developers.google.com/speed/webp

官方WIKI
https://developers.google.com/speed/webp/docs/api

GitHub
https://github.com/webmproject/libwebp

Google Git
https://chromium.googlesource.com/webm/libwebp

libwebp编译
https://blog.csdn.net/qq_36651243/article/details/120186555


## libwebp 解码过程详细的解释：

1.读取并解析文件头：
  
  libwebp 解码器首先读取 WebP 文件头的 RIFF 容器，从中提取出文件的基本信息，如图片宽度、高度、编码类型（有损或无损）、文件大小等。

2.根据编码类型进行解码：
 
  一：无损解码：
    
    a. 熵解码：使用算术编码或者 Huffman 编码进行熵解码。其目的是从压缩的数据中还原出原始的预测残差数据。
    
    b. 反向变换：无损编码中涉及到的变换包括颜色变换（例如，RGB 到 YUV）和空间变换（例如，利用 Median Edge Detection）。根据编码过程中使用的变换方式，解码器会反向执行这些变换来恢复原始的预测残差。
    
    c. 预测解码（重建）：解码器使用相邻像素的信息（例如，左边、上边和左上边像素）以及预测残差，通过逐个像素的预测恢复原始像素值。
  
  二：有损解码：
    
    a. 解码基本层：解码器使用 VP8 视频编解码器对基础层进行解码。VP8 是基于宏块的编码，包括帧内编码和帧间编码。解码器首先解码帧头，然后按照宏块的顺序对每个宏块进行解码。
    
    b. 解码增强层：增强层主要用于进一步压缩、存储和传输图片的细节信息。解码器在基础层的基础上对图片的细节进行修补和增强。
    
    c. 反量化：将量化后的 DCT 系数乘以反量化矩阵进行反量化操作，以还原原始的 DCT 系数。
    
    d. IDCT（反离散余弦变换）：使用反离散余弦变换将 DCT 系数变换回原始的图像像素值。

3.颜色空间转换：
 
  libwebp 默认在有损编码时使用 YUV 4:2:0 颜色空间。对于无损编码，libwebp 可以根据需要进行颜色空间转换。在解码结束后，需要将 YUV 数据转换回 RGB 数据，以便于应用程序和显示设备的使用。颜色空间转换涉及到 YUV 和 RGB 之间的转换公式，包括乘法和加法运算。

4.输出解码结果：
 
  解码器将解码后的像素数据存储在一个图像 buffer 中，并对 buffer 中的数据进行必要的后处理操作（例如，调整大小、缩放等）。最后将处理完的结果传递给应用程序或显示设备，实现对 WebP 图像的显示。
