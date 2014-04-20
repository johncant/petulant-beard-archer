
x = linspace(0,2, 200);
z = 20;
sigma = 0.2;
result = x;
reverse_x = x;
err = x;

for i=1:length(x)
  result(i) = zoom_func(x(i), z, sigma);
end

% We know x < f(x) for all x
% We know x > f(x)/z for all x
% No stationary points
% Monotonically increasing

x_est = result;
x_llims = result;
x_ulims = result;

for i=1:length(result)
  fx = result(i);

  x_ulim = fx*(1+(z-1)*exp(-0.5*(fx/sigma)^2));
  x_llim = fx;

  x_ulim_result = zoom_func(x_ulim, z, sigma);
  x_llim_result = zoom_func(x_llim, z, sigma);

  for j=1:10
    if fx > x_ulim_result
      x_ulim = 2 * x_ulim - x_llim;
    elseif fx < x_llim_result
      x_llim = 2 * x_llim - x_ulim;
    else
      test_x = 0.5*(x_ulim+x_llim);
      test_r = zoom_func(test_x, z, sigma);
      if test_r > fx
        x_ulim = test_x;
        x_ulim_result = test_r;
      else
        x_llim = test_x;
        x_llim_result = test_r;
      end
    end
  end

  x_ulims(i) = x_ulim;
  x_llims(i) = x_llim;
  x_est(i) = 0.5*(x_ulim+x_llim);
end


plot(result, [x_est - x]);
%plot(test_x, reverse_x);
