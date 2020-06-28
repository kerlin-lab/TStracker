import PIL.Image as IMG
import tifffile as tf

EXTENSION = '.tiff'

## File suffix
fname = '19144902_'
# fname = input('suffix of the file you want to dump: ')

## Number of ile with the suffix to dump
# num_file = int(input('How many file do you want to dump: '))
num_file = 1

## Getting tag list to dump
# tag_list = None
# while True:
# 	tag_list.append(input('Give me a tag (stop by typing enter):'))
# 	if tag_list[-1] == '':
# 		# Get rid of the last tag
# 		tag_list = tag_list[:-1]
# 		break
# 	else:
# 		# Use this if you are using Pillow lib, otherwise comment out the this else statement
# 		tag_list[-1] = int(tag_list[-1])

tag_list = ['DateTime','ImageDescription']

# Using Pillow lib
# img = IMG.open(fname+'_0'+EXTENSION)

# Using tifffile
for i in range(num_file):
	img = tf.TiffFile(fname+str(i)+EXTENSION)
	print('\n\nTiff file ' + str(i) + '\n')
	for p in range(len(img.pages)):
		print('\nPage ' + str(p))
		for tag in tag_list:
			print(tag + ': '+ str(img.pages[p].tags[tag].value))
		# input()
