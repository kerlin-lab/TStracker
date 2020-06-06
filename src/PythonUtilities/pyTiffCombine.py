import PIL.Image as IMG

EXTENSION = '.tiff'
fname = '19172444'
num_file = 5

tiffList = []

img = IMG.open(fname+'_0'+EXTENSION)
# Load all tiff files to RAM
print('Loading tiff files')
for i in range(1,num_file):
	tiffList.append(IMG.open(fname+'_'+str(i)+EXTENSION))

# Saving
print('Saving')
img.save(fname + EXTENSION,save_all = True, append_images = tiffList)

# Done
print('Done')