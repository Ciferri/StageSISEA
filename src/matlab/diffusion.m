function diffusion()
  clear all
  close all

  load('inPO2.dat');
  cmap=[0.5 0.5 0.5; 
      1 1 1; 
      0 0 1;
      1 0 0;
      0 0 0
      ];
  
  fid = fopen('tissueDim.dat', 'r');
  dim = fscanf(fid, '%i');
  y = dim(1);
  x = dim(2);
  z = dim(3);
  fclose(fid);
  
      
  for i=1:250
    matrix(:,:,i)=reshape(inPO2,x,y*z);
  end
    
  for i=1:250,   
    figure(1);
    A = matrix(:,:,i);
    B = imrotate(A,-90);
    C = flip(B,2);
    
    #c=mat2cell(C,[y y y],[x]);
    #subplot(3,1,1);imagesc(c{1,1});
    #subplot(3,1,2);imagesc(c{2,1});
    #subplot(3,1,3);imagesc(c{3,1});
    imagesc(C)
    #imagesc(matrix(:,:,i));
    #colormap('gray');
    img = ['/home/ciferri/Bureau/stage/v6/img/figure' num2str(i,'%04d') '.png'];
    saveas(1,img);
    #imwrite(C,cmap,img);
  end
  
  sum(sum(matrix(:,:,250)))/(x*y*z)
endfunction
