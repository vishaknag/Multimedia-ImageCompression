
import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.swing.*;
import java.nio.ByteBuffer;
import java.lang.Math;

public class imageReader 
{
 
	public int height = 288, width = 352;
	public double Y, U, V, Q;
	public byte[] bytesRGB, bytesYUV, bytesRGBOUT, bytesYUVSS, bytesYUVUS;
	public String RGBfileName;
	public File fp;
	public InputStream is;
	public byte rgbtoyuv = 1, yuvtorgb = 2;
	public byte printry = 0, mat = 1;
	public static byte inputRGB = 1, outputRGB = 2;
	public static int oldRGBnewRGB = 1, Ysampled_unsampled = 2, Usampled_unsampled = 3, Vsampled_unsampled = 4;
	
	public static void main(String[] args) 
	{
		imageReader imgObj = new imageReader();
		
		imgObj.ParseInput(args);
		
		imgObj.FillInputRGBSpace();
		
		imgObj.DisplayInputImage(inputRGB);
		
		imgObj.RGB2YUV();
		
		imgObj.SubSample();
		
		imgObj.UpSample();
		
		imgObj.YUV2RGB();
		
		imgObj.Print(Usampled_unsampled);
		
		imgObj.DisplayInputImage(outputRGB);
	}
	
	public void ParseInput(String[] args)
	{
		RGBfileName = args[0];
		Y = Integer.parseInt(args[1]);
		U = Integer.parseInt(args[2]);
		V = Integer.parseInt(args[3]);
		//Q = Integer.parseInt(args[4]);
	}
	
	public void DisplayInputImage(byte arrayToDisplay)
	{
		BufferedImage img = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
		
		try 
		{
			int ind = 0;
			for(int y = 0; y < height; y++)
			{
				for(int x = 0; x < width; x++)
				{
					if(arrayToDisplay == inputRGB)
					{
						byte a = 0;
						byte r = bytesRGB[ind];
						byte g = bytesRGB[ind+height*width];
						byte b = bytesRGB[ind+height*width*2]; 
					
						int pix = 0xff000000 | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
						img.setRGB(x,y,pix);
					}
					else if(arrayToDisplay == outputRGB)
					{
						byte a = 0;
						byte r = bytesRGBOUT[ind];
						byte g = bytesRGBOUT[ind+height*width];
						byte b = bytesRGBOUT[ind+height*width*2]; 
						
						int pix = 0xff000000 | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
						img.setRGB(x,y,pix);
					}
					
					ind++;
				}
			}
		} 
		catch (Exception e) 
		{
			e.printStackTrace();
		}
		
		// Use a label to display the image
		JFrame frame = new JFrame();
		JLabel label = new JLabel(new ImageIcon(img));
		frame.getContentPane().add(label, BorderLayout.CENTER);
		frame.pack();
		frame.setVisible(true);
	}
	
	public void FillInputRGBSpace()
	{
		try 
		{
			fp = new File(RGBfileName);
			is = new FileInputStream(fp);
			
			long len = fp.length();
			bytesRGB = new byte[(int)len];
			bytesYUV = new byte[(int)len];
			bytesRGBOUT = new byte[(int)len];
			bytesYUVSS = new byte[(int)len];
			bytesYUVUS = new byte[(int)len];
			
			int offset = 0;
			int numRead = 0;
			while (offset < bytesRGB.length && (numRead=is.read(bytesRGB, offset, bytesRGB.length-offset)) >= 0) 
			{
				offset += numRead;
			}
		}
		catch (FileNotFoundException e) 
		{
			e.printStackTrace();
		} 
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	public void RGB2YUV()
	{
		int ind = 0;
		for(int y = 0; y < height; y++)
		{
			for(int x = 0; x < width; x++)
			{
				byte floatRed = 0, floatGreen = 0, floatBlue = 0;
				floatRed = bytesRGB[ind];
				floatGreen = bytesRGB[ind+height*width];
				floatBlue = bytesRGB[ind+height*width*2]; 
				
				byte[] floatYUV = new byte[3];
				
				MatMult(floatRed, floatGreen, floatBlue, floatYUV, rgbtoyuv);
				
				// RGB -> YUV
				bytesYUV[ind] = floatYUV[0];
				bytesYUV[ind+height*width] = floatYUV[1];
				bytesYUV[ind+height*width*2] = floatYUV[2];
				
				// RGB -> YUV -> Sampled
				bytesYUVSS[ind] = floatYUV[0];
				bytesYUVSS[ind+height*width] = floatYUV[1];
				bytesYUVSS[ind+height*width*2] = floatYUV[2];
				
				// RGB -> YUV -> Sampled -> Upsampled
				bytesYUVUS[ind] = floatYUV[0];
				bytesYUVUS[ind+height*width] = floatYUV[1];
				bytesYUVUS[ind+height*width*2] = floatYUV[2];
				
				if(printry == 1)
				{
					System.out.println("R = "+(short)floatRed+"  Y = "+(short)(bytesYUV[ind] & 0xff));
					System.out.println("G = "+(short)floatGreen+"  U = "+(short)(bytesYUV[ind+height*width] & 0xff));
					System.out.println("B = "+(short)floatBlue+"  V = "+(short)(bytesYUV[ind+height*width*2] & 0xff)+"\n\n");
				}
				ind++;
			}
		}
	}
	
	public void YUV2RGB()
	{
		int ind = 0;
		for(int i = 0; i < height; i++)
		{
			for(int j = 0; j < width; j++)
			{
				byte floatY = 0, floatU = 0, floatV = 0;
				floatY = bytesYUVUS[ind];
				floatU = bytesYUVUS[ind+height*width];
				floatV = bytesYUVUS[ind+height*width*2];
				
				byte[] floatRGB = new byte[3];
				
				MatMult(floatY, floatU, floatV, floatRGB, yuvtorgb);
				
				bytesRGBOUT[ind] = floatRGB[0]; 
				bytesRGBOUT[ind+height*width] = floatRGB[1];
				bytesRGBOUT[ind+height*width*2] = floatRGB[2];
				
				if(printry == 1)
				{
					System.out.println("Y = "+(short)floatY+"  R = "+(short)(bytesRGB[ind] & 0xff));
					System.out.println("U = "+(short)floatU+"  G = "+(short)(bytesRGB[ind+height*width] & 0xff));
					System.out.println("V = "+(short)floatV+"  B = "+(short)(bytesRGB[ind+height*width*2] & 0xff)+"\n\n");
				}
				ind++;
			}
		}
	}
	
	/*-----------------------------------SUBSAMPLING-------------------------------------
	 
	 	Block Size: Number of pixels we are looking at each iteration of sampling
	 	
		Y, U & V  : the number of consecutive samples to be considered in each block
		
		Set "0" at all the pixels which are not a part of sub sampling along Y, U and V
	 
	 --------------------------------------------------------------------------------------*/
	public void SubSample()
	{
		// Total number of pixels in the image
		int pixelArrayLen = height * width, i = 0;
		
		// Number of pixels we are looking at in each iteration of SUB SAMPLING along each of Y U V
		double blockYSize = Math.pow(2.0, Y);
		double blockUSize = Math.pow(2.0, U);
		double blockVSize = Math.pow(2.0, V);
		
		//	Sampling Y
		i = 0;
		while(i < pixelArrayLen && Y != 0)
		{
			for(int blockIndex = (i + (int)Y); blockIndex < (i+(int)blockYSize); blockIndex++)
			{
				bytesYUVSS[blockIndex] = 0;
				bytesYUVUS[blockIndex] = 0;
			}
			
			i += (int)blockYSize;
		}
		
		// Sampling U
		i = pixelArrayLen;
		while(i < 2*pixelArrayLen && U != 0)
		{
			for(int blockIndex = (i + (int)U); blockIndex < (i+(int)blockUSize); blockIndex++)
			{
				bytesYUVSS[blockIndex] = 0;
				bytesYUVUS[blockIndex] = 0;
			}
			
			i += (int)blockUSize;
		}
		
		// Sampling V
		i = 2*pixelArrayLen;
		while(i < 3*pixelArrayLen && V != 0)
		{
			for(int blockIndex = (i + (int)V); blockIndex < (i+(int)blockVSize); blockIndex++)
			{
				bytesYUVSS[blockIndex] = 0;
				bytesYUVUS[blockIndex] = 0;
			}
			
			i += (int)blockVSize;
		}
	}
	
	/*------------------UPSAMPLING------------------------
	 
				  emptySlotCount (2)
					 ______
					 |	  |
			------------------------------------
			|x	|y	|?	|??	|z	|k	|	|	|p	|
			------------------------------------
				  |			  |
		emptyBlockStart    emptyBlockEnd   
	
		diffEnd_Start = z - y
	 
		stepAmount = diffEnd_Start / emptySlotCount
	 
		? = y + stepAmount
	 
		?? = ? + stepAmount
	-------------------------------------------------------*/
	public void UpSample()
	{
		// Total number of pixels in the image
		int pixelArrayLen = height * width, i = 0;
		int emptyBlockEnd, emptyBlockStart, emptySlotCount;
		byte diffEnd_Start, stepAmount;
		
		// Number of pixels we are looking at in each iteration of SUB SAMPLING along each of Y U V
		double blockYSize = Math.pow(2.0, Y);
		double blockUSize = Math.pow(2.0, U);
		double blockVSize = Math.pow(2.0, V);
		
		// Linear interpolation
		
		// Upsampling Y
		i = 0;
		while(i < pixelArrayLen && Y != 0)
		{
			emptyBlockEnd = i + (int)blockYSize;
			emptyBlockStart = i + ((int)Y - 1);
			emptySlotCount = (int)blockYSize - (int)Y;
			diffEnd_Start = (byte)(bytesYUVUS[emptyBlockEnd] - bytesYUVUS[emptyBlockStart]);
			stepAmount = (byte)(diffEnd_Start / emptySlotCount);
			
			for(int k = emptyBlockStart+1; k <= emptyBlockEnd-1; k++)
			{
				if(bytesYUV[k] != 0)
				   bytesYUVUS[k] = (byte)(bytesYUVUS[k-1] + stepAmount);
			}
			
			i += (int)blockYSize;
		}
		
		// Upsampling U
		i = pixelArrayLen;
		while(i < 2*pixelArrayLen && U != 0)
		{
			emptyBlockEnd = i + (int)blockUSize;
			emptyBlockStart = i + ((int)U - 1);
			emptySlotCount = (int)blockUSize - (int)U;
			diffEnd_Start = (byte)(bytesYUVUS[emptyBlockEnd] - bytesYUVUS[emptyBlockStart]);
			stepAmount = (byte)(diffEnd_Start / emptySlotCount);
			
			for(int k = emptyBlockStart+1; k <= emptyBlockEnd-1; k++)
			{
				if(bytesYUV[k] != 0)
					bytesYUVUS[k] = (byte)(bytesYUVUS[k-1] + stepAmount);
			}
			
			i += (int)blockUSize;
		}
		
		// Upsampling V
		i = 2*pixelArrayLen;
		while(i < (3*pixelArrayLen-blockVSize) && V != 0)
		{
			System.out.println("i = "+ i + "max = "+ (3*pixelArrayLen-1));
			emptyBlockEnd = i + (int)blockVSize;
			emptyBlockStart = i + ((int)V - 1);
			emptySlotCount = (int)blockVSize - (int)V;
			diffEnd_Start = (byte)(bytesYUVUS[emptyBlockEnd] - bytesYUVUS[emptyBlockStart]);
			stepAmount = (byte)(diffEnd_Start / emptySlotCount);
			
			for(int k = emptyBlockStart+1; k <= emptyBlockEnd-1; k++)
			{
				if(bytesYUV[k] != 0)
					bytesYUVUS[k] = (byte)(bytesYUVUS[k-1] + stepAmount);
			}
			
			i += (int)blockVSize;
		}
	}
	
	public void Print(int what)
	{
		int ind = 0;
		
		if(what == oldRGBnewRGB)
		{
			ind = 0;
			for(int i = 0; i < height; i++)
			{		
				for(int j = 0; j < width; j++)
				{
					if(bytesRGB[ind] != bytesRGBOUT[ind] || bytesRGB[ind+height*width] != bytesRGBOUT[ind+height*width] || bytesRGB[ind+height*width*2] != bytesRGBOUT[ind+height*width*2])
					{
						System.out.println("o R = "+bytesRGB[ind]+" n R = "+bytesRGBOUT[ind]);
						System.out.println("o G = "+bytesRGB[ind+height*width]+" n G = "+bytesRGBOUT[ind+height*width]);
						System.out.println("o B = "+bytesRGB[ind+height*width*2]+" n B = "+bytesRGBOUT[ind+height*width*2]+"\n\n");
					}
					ind++;
				}
			}
		}
			
		if(what == Ysampled_unsampled)
		{
			ind = 0;
			double blockYSize = Math.pow(2.0, Y);
			
			for(int i = 0; i < height; i++)
			{		
				for(int j = 0; j < width; j++)
				{
					if(ind%blockYSize == 0)
						System.out.println("\n\n");
					
					System.out.println("Original Y = "+bytesYUV[ind]+" Subsampled Y = "+bytesYUVSS[ind]+" Upsampled Y = "+bytesYUVUS[ind]);
				
					ind++;
				}
			}
		}
			
		if(what == Usampled_unsampled)
		{
			ind = 0;
			double blockUSize = Math.pow(2.0, U);
			
			for(int i = 0; i < height; i++)
			{		
				for(int j = 0; j < width; j++)
				{
					if(ind%blockUSize == 0)
						System.out.println("\n\n");
						
					System.out.println("Original U = "+bytesYUV[ind+height*width]+" Subsampled U = "+bytesYUVSS[ind+height*width]+" Upsampled U = "+bytesYUVUS[ind+height*width]);
					
					ind++;
				}
			}
		}
		
		if(what == Vsampled_unsampled)
		{
			ind = 0;
			double blockVSize = Math.pow(2.0, V);
			
			for(int i = 0; i < height; i++)
			{		
				for(int j = 0; j < width; j++)
				{
					if(ind%blockVSize == 0)
						System.out.println("\n\n");
					
					System.out.println("Original V = "+bytesYUV[ind+height*width*2]+" Subsampled V = "+bytesYUVSS[ind+height*width*2]+" Upsampled V = "+bytesYUVUS[ind+height*width*2]);
					
					ind++;
				}
			}
		}
			
			
	}
								 
									 
	// Matrix multiplication for converting between RGB space <-> YUV space
	public void MatMult(byte r_y, byte g_u, byte b_v, byte[] out, byte direction) 
	{
		double[] result = new double[3];
		
		// RGB -> YUV 
		if (direction == rgbtoyuv) 
		{
			if(mat == 1)
			{
				result[0] = (double)(0.299 * r_y + 0.587 * g_u + 0.114 * b_v);
				result[1] = (double)((-0.147 * r_y) + (-0.289 * g_u) + 0.436 * b_v); 
				result[2] = (double)(0.615 * r_y + (-0.515 * g_u) + 0.312 * b_v);
			}
			else
			{
				result[0] = (double)(0.2990 * r_y + 0.5870 * g_u + 0.1140 * b_v);
				result[1] = (double)((-0.1470 * r_y) + (-0.2890 * g_u) + 0.4360 * b_v); 
				result[2] = (double)(0.6150 * r_y + (-0.5140 * g_u) + 0.1000 * b_v);	
			}
		}
		
		// YUV -> RGB
		if(direction == yuvtorgb)
		{
			if(mat == 1)
			{
				result[0] = (double)(0.5303 * r_y + (-0.9545 * g_u) + 1.1401 * b_v);
				result[1] = (double)(1.2392 * r_y + 0.0915 * g_u + (-0.5806 * b_v)); 
				result[2] = (double)(1.0002 * r_y + 2.0324 * g_u + (-0.0005 * b_v));
			}
			else
			{
				result[0] = (double)(0.9989 * r_y + (0.0004 * g_u) + 1.1405 * b_v);
				result[1] = (double)(1.0006 * r_y + (-0.3948 * g_u) + (-0.5808 * b_v)); 
				result[2] = (double)(1.0000 * r_y + 2.0320 * g_u + (-0.0005 * b_v));
			}

		}
		
		/*
		// Wrap to the nearest integer
		for(byte i = 0; i < 3; i++)
		{	
			if(result[i] >= 0)
			{
				if(result[i]%1 >= 0.5)
					out[i] = (byte)Math.ceil(result[i]);
				else 
					out[i] = (byte)Math.floor(result[i]);
			}else{
				if(result[i]%1 < -0.5)
					out[i] = (byte)Math.floor(result[i]);
				else 
					out[i] = (byte)Math.ceil(result[i]);	
			}
		}*/
		
		/*
		int y = (int)Math.round(result[0]);
		if(y > 255)
			y = 255;
		if(y < 0)
			y = 0;
		
		int u = (int)Math.round(result[1]);
		if(u > 255)
			u = 255;
		if(u < 0)
			u = 0;
		
		int v = (int)Math.round(result[2]);
		if(v > 255)
			v = 255;
		if(v < 0)
			v = 0;
		*/
		
		out[0] = (byte)y;
		out[1] = (byte)u;
		out[2] = (byte)v;
	}
}