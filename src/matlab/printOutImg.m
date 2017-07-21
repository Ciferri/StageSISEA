clear all
close all
warning('off')
pkg load image

load('state.dat');
load('timer.dat');
cmap=[0.5 0.5 0.5; 
      1 1 1; 
      0 0 1;
      1 0 0;
      0 0 0
      ];

fid = fopen('tissueDim.dat', 'r');
dim = fscanf(fid, '%i');
nrow = dim(1);
ncol = dim(2);
nlayer = dim(3);
fclose(fid);

n3 = size(state, 1);
%figure(1)
for i = 1:n3
  for j = 1:nlayer
    mState(:,:,(i-1)*nlayer+j) = reshape(state(i, (j-1)*nrow*ncol+1:j*nrow*ncol), ncol, nrow);
    imgState = uint16(mState(:,:,(i-1)*nlayer+j)');
    %imshow(imgState, cmap)
    imgState = imresize(imgState,4,'nearest');
    pathState = ['imgState/figure' num2str((i-1)*nlayer+j,'%04d') '.png'];
    imwrite(imgState,cmap,pathState)

    mTimer(:,:,(i-1)*nlayer+j) = reshape(timer(i, (j-1)*nrow*ncol+1:j*nrow*ncol), ncol, nrow);
    imgTimer = double(mTimer(:,:,(i-1)*nlayer+j)'/1008);
    %imshow(imgTimer)
    imgTimer = imresize(imgTimer,4,'nearest');
    pathTimer = ['imgTimer/figure' num2str((i-1)*nlayer+j,'%04d') '.png'];
    imwrite(imgTimer, pathTimer)
  end
end
