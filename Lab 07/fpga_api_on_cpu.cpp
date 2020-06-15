#include "fpga_api.h"
#include <stdio.h>
#include <iostream>
#include <cstring>

using namespace std;

#define min(x, y) (((x) < (y)) ? (x) : (y))

FPGA::FPGA(off_t data_addr, off_t output_addr, int m_size, int v_size)
{
  m_size_ = m_size;
  v_size_ = v_size;
  data_size_ = (m_size_ + 1) * v_size_; // fpga bram data size

  output_ = new unsigned int[m_size_]; // use output_ as tempolar output
  data_ = new float[data_size_];

  num_block_call_ = 0;
}
FPGA::~FPGA()
{
  delete[] output_;
  delete[] data_;
}

float *FPGA::matrix(void)
{
  return data_ + v_size_;
}

float *FPGA::vector(void)
{
  return data_;
}

void FPGA::reset(void)
{
  num_block_call_ = 0;
}

int FPGA::num_block_call(void)
{
  return num_block_call_;
}

const float *FPGA::blockMV()
{
  num_block_call_ += 1;

  // cpu version
  float *vec = this->vector();
  float *mat = this->matrix();
  float *out = reinterpret_cast<float *>(output_);

  for (int i = 0; i < m_size_; ++i)
  {
    out[i] = 0;
    for (int j = 0; j < v_size_; ++j)
      out[i] += vec[j] * mat[v_size_ * i + j];
  }

  for (int i = 0; i < m_size_; ++i)
    data_[i] = out[i];

  return data_;
}

void FPGA::largeMV(const float *large_mat, const float *input, float *output, int num_input, int num_output)
{
  float *vec = this->vector();
  float *mat = this->matrix();

  // 0) Initialize output vector
  for (int i = 0; i < num_output; ++i)
    output[i] = 0;

  for (int i = 0; i < num_output; i += m_size_)
  {
    for (int j = 0; j < num_input; j += v_size_)
    {
      // 0) Initialize input vector
      int block_row = min(m_size_, num_output - i);
      int block_col = min(v_size_, num_input - j);

      // 1) Assign a vector
			/* IMPLEMENT */
			
			memset(vec, 0, sizeof(float) * v_size_);
			memcpy(vec, input + j, sizeof(float) * block_col);
			// for (int k = 0; k < block_col; k ++)
			// {
			// 	vec[k] = input[k+j];
			// }				

			// 2) Assign a matrix
			/* IMPLEMENT */
			/* for(int k = 0; k < block_row; k++) 
			{
				memcpy(mat + v_size_ * k, large_mat + k * num_input + i * num_input + j, sizeof(float) * block_col);
			} */

			for(int k = 0; k < block_row; k++)
            		{
                		for(int l=0; l < block_col; l++)
                		{
                    			mat[k * v_size_ + l] = large_mat[i * num_input + j + k * num_input + l];
                		}
            		}

      // 3) Call a function `blockMV() to execute MV multiplication
      const float *ret = this->blockMV();

      // 4) Accumulate intermediate results
      for (int row = 0; row < block_row; ++row)
        output[i + row] += ret[row];
    }
  }
}

void FPGA::convLowering(const std::vector<std::vector<std::vector<std::vector<float>>>> &cnn_weights,
                        std::vector<std::vector<float>> &new_weights,
                        const std::vector<std::vector<std::vector<float>>> &inputs,
                        std::vector<std::vector<float>> &new_inputs)
{
  /*
   * Arguments:
   *
   * conv_weights: [conv_channel, input_channel, conv_height, conv_width]
   * new_weights: [?, ?]
   * inputs: [input_channel, input_height, input_width]
   * new_inputs: [?, ?]
   *
   */

  int conv_channel = cnn_weights.size();
  int input_channel = cnn_weights[0].size();
  int conv_height = cnn_weights[0][0].size();
  int conv_width = cnn_weights[0][0][0].size();
  //int input_channel = inputs.size();
  int input_height = inputs[0].size();
  int input_width = inputs[0][0].size();

  // IMPLEMENT THIS
  // For example,
  // new_weights[0][0] = cnn_weights[0][0][0][0];
  // new_inputs[0][0] = inputs[0][0][0];

  // printf("%d, %d, %d, %d\n", conv_channel, input_channel, conv_height, conv_width);
  //new weights
  for (int i = 0; i < conv_channel; i++){
    for (int j = 0; j < input_channel; j++){
      for (int k = 0; k < conv_height; k++){
        for(int l = 0; l < conv_width; l++){
          new_weights[i][(j * conv_height * conv_width) + (k * conv_width) + l] = cnn_weights[i][j][k][l];
        }
      }
    }
  }
  

  //new inputs
  int col_filt = input_height - conv_height + 1;
  int row_filt = input_width - conv_width + 1;
  
  // printf("%d, %d\n", col_filt, row_filt);
  // printf("%d, %d\n", input_height, input_width);


  for( int rgb = 0; rgb < input_channel; rgb++){
    for (int row = 0; row < row_filt; row++){
      for(int col = 0; col < col_filt; col++){
        for(int h = 0; h < conv_height; h++){
          for(int w = 0; w < conv_width; w++){
            new_inputs[rgb * conv_height * conv_width + h * conv_width + w][row * col_filt + col] = inputs[rgb][row + h][col + w];
          }
        }
      }
    }
  }
}