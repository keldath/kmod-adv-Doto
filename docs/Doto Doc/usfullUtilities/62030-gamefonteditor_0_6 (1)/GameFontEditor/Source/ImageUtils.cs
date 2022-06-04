using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;

namespace GameFontEditor
{
    public static class ImageUtils
    {
        public struct ImageData
        {
            public int Width;
            public int Height;
            public int[] Data;

            public Color GetPixel(int x, int y)
            {
                return Color.FromArgb(Data[y * Width + x]);
            }

            public void SetPixel(int x, int y, Color color)
            {
                Data[y * Width + x] = color.ToArgb();
            }

            public ImageData Clone()
            {
                ImageData other = new ImageData();
                other.Width = Width;
                other.Height = Height;
                other.Data = new int[Width * Height];
                Array.Copy(Data, other.Data, Width * Height);
                return other;
            }
        }

        public static ImageData GetDataFromImage(Bitmap bmp)
        {
            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
            BitmapData data = bmp.LockBits(rect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            if (data.Stride != data.Width * 4)
            {
                MessageBox.Show("Stride is not a multiple of width!", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
            int ints = bmp.Width * bmp.Height;
            int[] rgbValues = new int[ints];
            System.Runtime.InteropServices.Marshal.Copy(data.Scan0, rgbValues, 0, ints);
            bmp.UnlockBits(data);
            ImageData imgData = new ImageData();
            imgData.Width = bmp.Width;
            imgData.Height = bmp.Height;
            imgData.Data = rgbValues;
            return imgData;
        }

        public static Bitmap GetImageFromData(ImageData data)
        {
            Bitmap result = new Bitmap(data.Width, data.Height);
            Rectangle resultRect = new Rectangle(0, 0, data.Width, data.Height);
            BitmapData resultData = result.LockBits(resultRect, ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
            System.Runtime.InteropServices.Marshal.Copy(data.Data, 0, resultData.Scan0, data.Width * data.Height);
            result.UnlockBits(resultData);
            return result;
        }

        public static Bitmap BuildSubImage(Bitmap bmp, Rectangle rect)
        {
            return BuildSubImage(GetDataFromImage(bmp), rect);
        }

        public static Bitmap BuildSubImage(ImageData imgData, Rectangle rect)
        {
            Bitmap result = new Bitmap(rect.Width, rect.Height);

            int ints = result.Width * result.Height;
            int[] rgbValues = new int[ints];

            // Highly inefficient, but oh well
            for (int line = 0; line < result.Height; ++line)
            {
                for (int col = 0; col < result.Width; ++col)
                {
                    int nSrcIndex = (line + rect.Top) * imgData.Width + col + rect.Left;
                    int nDstIndex = line * result.Width + col;
                    rgbValues[nDstIndex] = imgData.Data[nSrcIndex];
                }
            }

            Rectangle resultRect = new Rectangle(0, 0, rect.Width, rect.Height);
            BitmapData resultData = result.LockBits(resultRect, ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
            System.Runtime.InteropServices.Marshal.Copy(rgbValues, 0, resultData.Scan0, ints);
            result.UnlockBits(resultData);
            return result;
        }

        public static int CalcColorDistance(Color c1, Color c2)
        {
            // No alpha
            return Math.Abs((short)c1.R - (short)c2.R) + Math.Abs((short)c1.G - (short)c2.G) + Math.Abs((short)c1.B - (short)c2.B);
        }

        public static int PasteColor(int fg, int bg)
        {
            // Highly inefficient, but it's done to small portions of the image each time
            Color bgColor = Color.FromArgb(bg);
            Color fgColor = Color.FromArgb(fg);

            if (fgColor.A == 255)
                return fg;

            if (fgColor.A == 0)
                return bg;

            float fgAlpha = fgColor.A / 255.0f;

            // Blend (the bg alpha doesn't matter)
            byte a = Math.Max(fgColor.A, bgColor.A);
            byte r = (byte)(fgAlpha * fgColor.R + (1 - fgAlpha) * bgColor.R);
            byte g = (byte)(fgAlpha * fgColor.G + (1 - fgAlpha) * bgColor.G);
            byte b = (byte)(fgAlpha * fgColor.B + (1 - fgAlpha) * bgColor.B);
            return Color.FromArgb(a, r, g, b).ToArgb();
        }

        public static Bitmap PasteImage(Bitmap fg, Bitmap bg)
        {
            ImageData fgData = GetDataFromImage(fg);
            ImageData bgData = GetDataFromImage(bg);

            for (int y = 0; y < bgData.Height; ++y)
            {
                for (int x = 0; x < bgData.Width; ++x)
                {
                    int index = y * bgData.Width + x;
                    bgData.Data[index] = PasteColor(fgData.Data[index], bgData.Data[index]);
                }
            }

            return GetImageFromData(bgData);
        }

        public static bool CreateImageWithTransparency(Bitmap orig, out Bitmap result, int nSensitivity)
        {
            result = null;

            ImageUtils.ImageData data = ImageUtils.GetDataFromImage(orig);

            Color bgColor;

            Color corner1OpaqueColor = Color.FromArgb(255, data.GetPixel(0, 0));
            Color corner2OpaqueColor = Color.FromArgb(255, data.GetPixel(0, data.Height - 1));
            Color corner3OpaqueColor = Color.FromArgb(255, data.GetPixel(data.Width - 1, 0));
            Color corner4OpaqueColor = Color.FromArgb(255, data.GetPixel(data.Width - 1, data.Height - 1));

            // Make sure all corners are of a similar color (distance from average < sensitivity)
            // and use the average as the background color
            // (An ugly code, but oh well)
            byte avgR = (byte)(((int)corner1OpaqueColor.R + (int)corner2OpaqueColor.R + (int)corner3OpaqueColor.R + (int)corner4OpaqueColor.R) / 4);
            byte avgG = (byte)(((int)corner1OpaqueColor.G + (int)corner2OpaqueColor.G + (int)corner3OpaqueColor.G + (int)corner4OpaqueColor.G) / 4);
            byte avgB = (byte)(((int)corner1OpaqueColor.B + (int)corner2OpaqueColor.B + (int)corner3OpaqueColor.B + (int)corner4OpaqueColor.B) / 4);

            bgColor = Color.FromArgb(avgR, avgG, avgB);

            if (CalcColorDistance(corner1OpaqueColor, bgColor) > nSensitivity ||
                CalcColorDistance(corner2OpaqueColor, bgColor) > nSensitivity ||
                CalcColorDistance(corner3OpaqueColor, bgColor) > nSensitivity ||
                CalcColorDistance(corner4OpaqueColor, bgColor) > nSensitivity)
            {
                Program.Log("Error: All corners must be of a similar color to automatically create transparency. Try increasing the sensitivity, if possible.");
                return false;
            }

            bool bAlphaExists = false;
            for (int x = 0; x < data.Width && !bAlphaExists; ++x)
            {
                for (int y = 0; y < data.Height && !bAlphaExists; ++y)
                {
                    if (data.GetPixel(x, y).A != 255)
                    {
                        bAlphaExists = true;
                        break;
                    }
                }
            }

            if (bAlphaExists)
                Program.Log("Warning: slot image already contains alpha channel. Overriding alpha... (You can undo if needed)");

            // calc alpha
            for (int x = 0; x < data.Width; ++x)
            {
                for (int y = 0; y < data.Height; ++y)
                {
                    Color pixel = data.GetPixel(x, y);
                    Color opaquePixel = Color.FromArgb(255, pixel);
                    int alpha = 255;
                    if (nSensitivity > 0)
                    {
                        int nDist = CalcColorDistance(opaquePixel, bgColor);
                        if (nDist <= nSensitivity)
                        {
                            alpha = nDist;
                        }
                        else
                            alpha = 255;
                    }
                    else
                    {
                        if (opaquePixel == bgColor)
                            alpha = 0;
                        else
                            alpha = 255;
                    }

                    pixel = Color.FromArgb(alpha, opaquePixel);
                    data.SetPixel(x, y, pixel);
                }
            }

            result = ImageUtils.GetImageFromData(data);
            return true;
        }
    }
}
