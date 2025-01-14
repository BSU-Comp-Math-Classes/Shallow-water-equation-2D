clear
disp("reading data... (may take a while)")
[x,y,h] = importfile1("sw2d_mpi_init.dat");

N = sqrt(length(x))
h_array = reshape(h,N,N);
%uh_array = reshape(uh,N,N);
%vh_array = reshape(vh,N,N);

X = reshape(x,N,N);
Y = reshape(y,N,N);
disp('plotting...')
%contourf(X,Y,h_array)
s = surf(X,Y,h_array,'FaceAlpha',0.7);
s.EdgeColor = 'none';
xlabel("X")
ylabel("Y")
title("Wave height")
colorbar