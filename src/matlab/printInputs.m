clear all
close all

warning('off')

pkg load image

tTissueName = {'Tissu P14 01685 6-1-cd31 101x62';   
'Tissu P14 01685 7-5-cd31 97x57';    
'Tissu P14 01685 8-5-cd31 111x75';   
'Tissu P14 08021 7-2-cd31 125x73';   
'Tissu P14 08021 8-2-cd31 120x71';   
'Tissu P14 08021 9-2-cd31 91x55';    
'Tissu P14 12769 12-2-cd31 97x56';   
'Tissu P14 12769 13-4-cd31 108x65';  
'Tissu P14 12769 14-2-cd31 109x64';  
'Tissu P14 20984 5-1-cd31 95x58';    
'Tissu P14 20984 6-1-cd31 119x70';
'Tissu P14 20984 7-1-cd31 107x66';
'Tissu P14 26519 5-3-cd31 93x55';
'Tissu P14 26519 6-3-cd31 55x37';
'Tissu P14 26519 7-3-cd31 95x59';
'Tissu P14 26671 4-1-cd31 94x58';
'Tissu P14 26671 5-3-cd31 104x65';
'Tissu P14 26671 6-1-cd31 98x63';
'Tissu P14 29860 4-1-cd31 101x56';
'Tissu P14 29860 5-1-cd31 113x64';
'Tissu P14 29860 6-1-cd31 125x71'};
 
tTissueDim = [101, 62, 1;
97, 57, 1;
111, 75, 1;
125, 73, 1;
120, 71, 1;
91, 55, 1;
97, 56, 1;
108, 65, 1;
109, 64, 1;
95, 58, 1;
119, 70, 1;
107, 66, 1;
93, 55, 1;
55, 37, 1;
95, 59, 1;
94, 58, 1;
104, 65, 1;
98, 63, 1;
101, 56, 1;
113, 64, 1;
125, 71, 1];

sel = input(['Select tissue (from 1 to ',num2str(size(tTissueName,1)),') ']);
tissueName = tTissueName{sel};
nrow = tTissueDim(sel, 2);
ncol = tTissueDim(sel, 1);
nlayer = tTissueDim(sel, 3);
img = imread(['./src/data/initTissues/', tissueName, '.png']);

fid = fopen('./src/data/tissueDim.dat', 'w');
fprintf(fid, '%i\n%i\n%i\n', nrow, ncol, nlayer);
fclose(fid);

img = imresize(img, [nrow, ncol]);
imgB = img(:, :, 3);

initVes = imgB >= 0 & imgB < 80;
fid = fopen('./src/data/inVes.dat', 'w');
for k = 1:nlayer
  for i = 1:nrow
    for j = 1:ncol
      fprintf(fid, '%f\n', initVes(i, j));
    end
  end
end
fclose(fid);

initTum = imgB > 80 & imgB < 200;
fid = fopen('./src/data/inTum.dat', 'w');
for k = 1:nlayer
  for i = 1:nrow
    for j = 1:ncol
      if(k == ceil(nlayer / 2))
        fprintf(fid, '%f\n', initTum(i, j));
      else
      fprintf(fid, '%f\n', 0);
      endif
    end
  end
end
fclose(fid);

PO2 = load(['./src/data/initPO2/', tissueName, '.dat']);
n3 = size(PO2, 1);
fid = fopen('./src/data/inPO2.dat', 'w');
initPO2 = reshape(PO2(n3, :), ncol, nrow)';
for k = 1:nlayer
  for i = 1:nrow
    for j = 1:ncol
      fprintf(fid, '%f\n', initPO2(i, j));
    end
  end
 end
fclose(fid);
