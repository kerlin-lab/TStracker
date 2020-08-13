import tifffile as tf
import os


## File path config
img_folder  = 'D:\\Temp\\3 002072311001-2\\8-3-20\\'
fname       = '19144903_trial_1_'
output_folder = 'D:\\Output\\3 002072311001-2\\8-3-20\\'





def extract_tags(tags):
  """
    Extra description and datetime tags
  """
  extra_tags_list = []
  for t in tags:
    if t.code == 306:
      # Only extract the DateTime and the Description tag
      extra_tags_list.append((t.code,t.dtype if t.dtype[0] != '1' else t.dtype[1:] ,t.count,t.value,False))
  return extra_tags_list

EXTENSION = '.tiff'

# Variables
out_fname = fname[:-2]
BREAK_DURATION = 1000000000  # 1 s = 1000,000,000 nano sec
counter = 0
last_time = 0
this_time = 0
trial_counter = 1
file_index_counter = 0
# Check and create output folder
try:
  os.listdir(output_folder)
except:
  print(f'Output folder has not been created\nCreating folder {output_folder}')
  try:
    os.makedirs(output_folder)
  except Exception as e:
    print(f'Error trying creating output folder {e}')
    exit(1)
# Open tiff file to write
out_img = tf.TiffWriter(
  file=f'{output_folder}{out_fname}{str(trial_counter)}{EXTENSION}',
  append=True
)
# Using tifffile
while True:
  try:
    full_file_name = f'{img_folder}{fname}{str(file_index_counter)}{EXTENSION}'
    print(f'Analyzing file {full_file_name}')
    with tf.TiffFile(full_file_name) as img:
      for p in range(len(img.pages)):
        print(f'Processing page {p}')
        # Get tags data
        tags = img.pages[p].tags
        # Load the image pixel data of this frame to memory
        img_data = tf.memmap(
          filename=full_file_name,
          page=p,
          mode='r'    # Read only
        )
        # Detecting trial break
        this_time = int(tags['DateTime'].value)
        diff = this_time - last_time
        if diff > BREAK_DURATION:
          # Break detected
          out_img.close()         # Close current file
          trial_counter += 1      # Increment trial counter
          # Open new Tifffile to write to
          out_img = tf.TiffWriter(
            file=f'{output_folder}{out_fname}{str(trial_counter)}{EXTENSION}',
            append=True
          )
        # Append tiff page to file
        print(extract_tags(tags))
        out_img.save(
          data=img_data,
          extratags=extract_tags(tags),
          contiguous=False,
          description=tags['ImageDescription'].value
        )
        # Reset time holder
        last_time = this_time
      # Move to next file when done
      file_index_counter+=1
  except Exception as e:
    # No more file to read stop
    if trial_counter == 1:
      print(f'Error {e}')
    else:
      print(f'Done separating trial')
    break
# Close the last tiff file
out_img.close()
# Announce
print('========================================')
print(f'Number trial found: {trial_counter}')
print(f'Output folder: {output_folder}')
