#include "seam_carver.hpp"
#include <iostream>
// implement the rest of SeamCarver's functions here

const ImagePPM& SeamCarver::GetImage() const {
  return image_;
}
int SeamCarver::GetHeight() const {
  return height_;
}
int SeamCarver::GetWidth() const {
  return width_;
}
int SeamCarver::GetEnergy(int row, int col) const {
  int r_row = 0;  int b_row = 0;  int g_row = 0;  int r_col = 0;  int b_col = 0;
  int g_col = 0;
  int c = 0;
  int input = 0;
  if(row == 0) {
    input = height_ - 1;
    c = col;
    r_row = image_.GetPixel(height_ - 1, col).GetRed() - image_.GetPixel(row + 1, col).GetRed();
    b_row = image_.GetPixel(height_ - 1, col).GetBlue() - image_.GetPixel(row + 1, col).GetBlue();
    g_row = image_.GetPixel(height_ - 1, col).GetGreen() - image_.GetPixel(row + 1, col).GetGreen();
    c = 0;
    input = 0;
  } else if (row == height_ - 1) {
    r_row = image_.GetPixel(row - 1, col).GetRed() - image_.GetPixel(0, col).GetRed();
    b_row = image_.GetPixel(row - 1, col).GetBlue() - image_.GetPixel(0, col).GetBlue();
    g_row = image_.GetPixel(row - 1, col).GetGreen() - image_.GetPixel(0, col).GetGreen();
  } else {
    r_row = image_.GetPixel(row - 1, col).GetRed() - image_.GetPixel(row + 1, col).GetRed();
    b_row = image_.GetPixel(row - 1, col).GetBlue() - image_.GetPixel(row + 1, col).GetBlue();
    g_row = image_.GetPixel(row - 1, col).GetGreen() - image_.GetPixel(row + 1, col).GetGreen();
  }
  if (col == 0) {
    r_col = image_.GetPixel(row, width_ - 1).GetRed() - image_.GetPixel(row, col + 1).GetRed();
    b_col = image_.GetPixel(row, width_ - 1).GetBlue() - image_.GetPixel(row, col + 1).GetBlue();
    g_col = image_.GetPixel(row, width_ - 1).GetGreen() - image_.GetPixel(row, col + 1).GetGreen();
  } else if (col == width_ - 1) {
    r_col = image_.GetPixel(row, col - 1).GetRed() - image_.GetPixel(row, 0).GetRed();
    b_col = image_.GetPixel(row, col - 1).GetBlue() - image_.GetPixel(row, 0).GetBlue();
    g_col = image_.GetPixel(row, col - 1).GetGreen() - image_.GetPixel(row, 0).GetGreen();
  } else {
    r_col = image_.GetPixel(row, col - 1).GetRed() - image_.GetPixel(row, col + 1).GetRed();
    b_col = image_.GetPixel(row, col - 1).GetBlue() - image_.GetPixel(row, col + 1).GetBlue();
    g_col = image_.GetPixel(row, col - 1).GetGreen() - image_.GetPixel(row, col + 1).GetGreen();
  }  

  int delta_col = (r_col * r_col) + (g_col * g_col) + (b_col * b_col);
  int delta_row = (r_row * r_row) + (g_row * g_row) + (b_row * b_row);
  r_col = c + input;
  return delta_col + delta_row;
}
int Min (int one, int two, int three) {
  if (one <= two && one <= three) {
    return one;
  }
  if (two <= three) {
    return two;
  }
  return three;
}
int Min (int one, int two) {
  if (one <= two) {
    return one;
  }
  return two;
}
int* SeamCarver::HorizHelper(int start, int** values) const {
  int* ret = new int[width_];
  ret[0] = start;
  int row = start;
  for (int i = 1; i < width_; i++) {
    int straight = values[row][i];
    if (row == 0) {
      int plus = values[row + 1][i];

      if (straight <= plus) {
        ret[i] = row;
      } else {
        ret[i] = row + 1;
        row++;
      }
    } else if (row == height_ - 1) {
      int minus = values[row - 1][i];
      if (straight <= minus) {
        ret[i] = row;
      } else {
        ret[i] = row - 1;
        row--;
      }
    } else {
      int plus = values[row + 1][i];
      int minus = values[row - 1][i];
      if (straight <= plus && straight <= minus) {
        ret[i] = row;
      } else if (minus <= plus) {
        ret[i] = row - 1;
        row--;
      }else {
        ret[i] = row + 1;
        row++;
      }
    }
  }
  return ret;
}
int* SeamCarver::GetHorizontalSeam() const { 
  int** values = new int*[height_];
    for (int i = 0; i < height_; i++) {
      values[i] = new int[width_];
    }
    for(int row = 0; row < height_; row++) {
      values[row][width_ - 1] = GetEnergy(row,width_ - 1);
    }
    for (int col = width_ - 2; col >= 0; col--) {
      for(int row = 0; row < height_; row++) {
        int best = 0;
        if (row == 0) {
          best = Min(values[row][col + 1], values[row + 1][col + 1]);
        } else if (row == height_ - 1) {
          best = Min(values[row][col + 1], values[row - 1][col + 1]);
        } else {
          best = Min(values[row][col + 1], values[row + 1][col + 1], values[row - 1][col + 1]);
        }
        values[row][col] = best + GetEnergy(row,col);
      }
    }
    int min = kMax;
    int index = 0;
    for (int row = 0; row < height_; row++) {
      if (values[row][0] < min) {
        min = values[row][0];
        index = row;
      }
    }
    int* ret = HorizHelper(index, values);
    for (int i = 0; i < height_; i++) {
      delete[] values[i];
      values[i] = nullptr;
    }
    delete[] values;
    values = nullptr;
    return ret;
}
int* SeamCarver::VertHelper(int start, int** values) const {
  int* ret = new int[height_];
  ret[0] = start;
  int col = start;
  for (int i = 1; i < height_; i++) {
    int straight = values[i][col];
    if (col == 0) {
      int plus = values[i][col + 1];
      if (straight <= plus) {
        ret[i] = col;
      } else {
        ret[i] = col + 1;
        col++;
      }
    } else if (col == width_ - 1) {
      int minus = values[i][col - 1];
      if (straight <= minus) {
        ret[i] = col;
      } else {
        ret[i] = col - 1;
        col--;
      }
    } else {
      int plus = values[i][col + 1];
      int minus = values[i][col - 1];
      if (straight <= plus && straight <= minus) {
        ret[i] = col;
      } else if (minus <= plus && minus <= straight) {
        ret[i] = col - 1;
        col--;
      }else {
        ret[i] = col + 1;
        col++;
      }
    }
  }
  return ret;
}
int* SeamCarver::GetVerticalSeam() const {
    int** values = new int*[height_];
    for (int i = 0; i < height_; i++) {
      values[i] = new int[width_];
    }
    for(int col = 0; col < width_; col++) {
      values[height_ - 1][col] = GetEnergy(height_ - 1,col);
    }
    for (int row = height_ - 2; row >= 0; row--) {
      for(int col = 0; col < width_; col++) {
        int best = 0;
        if (col == 0) {
          best = Min(values[row + 1][col + 1], values[row + 1][col]);
        } else if (col == width_ - 1) {
          best = Min(values[row + 1][col], values[row + 1][col - 1]);
        } else {
          best = Min(values[row + 1][col + 1], values[row + 1][col], values[row + 1][col - 1]);
        }
        values[row][col] = best + GetEnergy(row,col);
      }
    }
    int min = kMax;
    int index = 0;
    for (int col = 0; col < width_; col++) {
      if (values[0][col] < min) {
        min = values[0][col];
        index = col;
      }
    }
    int* ret = VertHelper(index, values);
    for (int i = 0; i < height_; i++) {
      delete[] values[i];
      values[i] = nullptr;
    }
    delete[] values;
    values = nullptr;
    return ret;
}
void SeamCarver::RemoveHorizontalSeam() {
  int* seam = GetHorizontalSeam();
  auto** hold = new Pixel*[height_ - 1];
  for (int i = 0; i < height_ - 1; i++) {
    hold[i] = new Pixel[width_];
  }
  int c = -1;
  for (int col = 0; col < width_; col++) {
    c++;
    int r = -1;
    for (int row = 0; row < height_; row++) {
      r++;
        if (seam[col] == row) {
          r--;
        } else {
          hold[r][c] = image_.GetPixel(row,col);
        }
    }
  }
  image_.Overwrite(hold, false);
  delete[] seam;
  height_--;
}
void SeamCarver::RemoveVerticalSeam() { 
  int* seam = GetVerticalSeam();
  auto** hold = new Pixel*[height_];
  for (int i = 0; i < height_; i++) {
    hold[i] = new Pixel[width_ - 1];
  }
  int r = -1;
  for (int row = 0; row < height_; row++) {
    r++;
    int c = -1;
    for (int col = 0; col < width_; col++) {
      c++;
        if (seam[row] == col) {
          c--;
        } else {
          hold[r][c] = image_.GetPixel(row,col);
        }
    }
  }
  image_.Overwrite(hold, true);
  delete[] seam;
  width_--;
}
// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM& image): image_(image) {
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM& image) {
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}
