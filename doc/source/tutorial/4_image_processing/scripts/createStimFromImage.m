function createStimFromImage(inFile, outFile)
if nargin<2,outFile='../input/image.dat';end

pic = PictureStim(inFile);
pic.resize([256 256]);
pic.rgb2gray();
pic.save(outFile);

end