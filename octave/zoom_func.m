function res = zoom_func(x, z, sigma)
  res = x/(1+(z-1)*exp(-0.5*x^2/(sigma^2)));
end

