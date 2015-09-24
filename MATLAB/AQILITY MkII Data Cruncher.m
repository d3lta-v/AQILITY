% AQILITY MkII Data Cruncher v0.1
% Language: MATLAB Scientific Processing Language

% This program serves to crunch numbers for the raw data coming from AQILITY
% and put that processed data into a new ThingSpeak channel that provides
% accurate data and modeling due to the new, processed data.

% For reference:
% Raw data channel: https://thingspeak.com/channels/31799
% Processed data channel: https://thingspeak.com/channels/56281

readChannelID = 31799;

writeChannelID = 56281;
writeAPIKey = []; % API key is redacted to prevent abuse

%% Read Data %%
[raw] = thingSpeakRead(readChannelID,'Fields',[1 2],'NumPoints',5,'OutputFormat','table');
% Retrieve mean, with pm25avg being multiplied by 10 sue to an error in
% calculations made inside the AVR chip
pm25avg = mean(raw.PM25ugm3) * 10;
pm10avg = mean(raw.PM10ugm3);

%% Analyze Data %%

% Init aqi25 and aqi10, AQI based on respective ug/m^3 raw values
aqi25 = 0;
aqi10 = 0;
% Init upper and lower bounds of categories.
% Note: Equation for AQI is a piecewise linear function, represented in
% I = ((iHigh - iLow) / (cHigh - cLow)) * (c - cLow) + iLow
% Where:
% iHigh/iLow: Upper and lower bounds of the indices (e.g. 100-200)
% cHigh/cLow: Upper and lower bounds of the concentrations (e.g. 12.1-35.4)
% c: Current concentration of pollutant (pm25avg or pm10avg)
iHigh = 0;
iLow = 0;
cHigh = 0;
cLow = 0;

% Cascading if conditionals for the piecewise linear function (PM2.5)
if (pm25avg >= 350.5) && (pm25avg <= 500.4)
  cLow = 350.5;
  cHigh = 500.4;
  iLow = 401;
  iHigh = 500;
elseif (pm25avg >= 250.5) && (pm25avg <= 350.4)
  cLow = 250.5;
  cHigh = 350.4;
  iLow = 301;
  iHigh = 400;
elseif (pm25avg >= 150.5) && (pm25avg <= 250.4)
  cLow = 150.5;
  cHigh = 250.4;
  iLow = 201;
  iHigh = 300;
elseif (pm25avg >= 55.5) && (pm25avg <= 150.4)
  cLow = 55.5;
  cHigh = 150.4;
  iLow = 151;
  iHigh = 200;
elseif (pm25avg >= 35.5) && (pm25avg <= 55.4)
  cLow = 35.5;
  cHigh = 55.4;
  iLow = 101;
  iHigh = 150;
elseif (pm25avg >= 12.1) && (pm25avg <= 35.4)
  cLow = 12.1;
  cHigh = 35.4;
  iLow = 51;
  iHigh = 100;
elseif (pm25avg <= 12)
  cLow = 0;
  cHigh = 12;
  iLow = 0;
  iHigh = 50;
else
  error('PM2.5 out of bounds')
end

% Write into PM2.5 variable for final output
aqi25 = ((iHigh - iLow) / (cHigh - cLow)) * (pm25avg - cLow) + iLow

% Reset calculation variables
iLow = 0;
iHigh = 0;
cLow = 0;
cHigh = 0;

% Cascading if conditionals for the piecewise linear function (PM10)
if pm10avg >= 505 && pm10avg <= 604
  cLow = 505;
  cHigh = 604;
  iLow = 401;
  iHigh = 500;
elseif pm10avg >= 425 && pm10avg <= 504
  cLow = 425;
  cHigh = 504;
  iLow = 301;
  iHigh = 400;
elseif pm10avg >= 355 && pm10avg <= 424
  cLow = 355;
  cHigh = 424;
  iLow = 201;
  iHigh = 300;
elseif pm10avg >= 255 && pm10avg <= 354
  cLow = 255;
  cHigh = 354;
  iLow = 151;
  iHigh = 200;
elseif pm10avg >= 155 && pm10avg <= 254
  cLow = 155;
  cHigh = 254;
  iLow = 101;
  iHigh = 150;
elseif pm10avg >= 55 && pm10avg <= 154
  cLow = 55;
  cHigh = 154;
  iLow = 51;
  iHigh = 100;
elseif pm10avg <= 54
  cLow = 0;
  cHigh = 54;
  iLow = 0;
  iHigh = 50;
else
  error('PM10 out of bounds')
end

% Write into PM10 variable for final output
aqi10 = ((iHigh - iLow) / (cHigh - cLow)) * (pm10avg - cLow) + iLow

%% Writing Data %%

analyzedData = [aqi25, aqi10, pm25avg, pm10avg];
thingSpeakWrite(writeChannelID, analyzedData, 'WriteKey', writeAPIKey);
