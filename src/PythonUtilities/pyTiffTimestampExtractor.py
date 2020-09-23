import tifffile as tf
import os

def extract_timestamp(file_name, tag_name='DateTime'):
  tiff = tf.TiffFile(file_name)
  timestamp_list = []
  for p in range(len(tiff.pages)):
    timestamp_list.append(int(tiff.pages[p].tags[tag_name].value))
  return timestamp_list

def extract_all_timestamp_from_folder(folder_path, file_name_structure, first_trial_index = 1, verbose = False):
  """
    This function takes in a path to folder containing the tiff file and returns a dictionary contains timestamp of all the image, of which name matches the given strucutre, in the folder 
    @param folder_path: The path to the folder
    @param file_name_structure: The file name structure of image file inside the folder
      Ex: if the images inside the folder are
        19144902_trial_1.tiff
        19144902_trial_2.tiff
        19144902_trial_3.tiff
        ...
      so these files have the pattern 
      19144902_trial_ + TRIAL_NUMBER + .tiff
      to describe the structure to the function, replace where the TRAIL_NUMBER is at with 
      a pair of {}
      With the above structure, the descriptor will be

      19144902_trial_{}.tiff

      @ATTENTION: this function does not take into account files from different cameras with different serial number,
      user need to account for this by themself
      For example: In the folder /data/ , there are 3 images from camera 19144902 and 3 images from 19144903,
      so the file list looks like
        19144902_trial_1.tiff
        19144902_trial_2.tiff
        19144902_trial_3.tiff

        19144903_trial_1.tiff
        19144903_trial_2.tiff
        19144903_trial_3.tiff

        When user calls extract_all_timestamp_from_folder('/data/', '19144902_trial_{}.tiff')
        this functions only extracts the timestamp from 19144902_trial_1.tiff to 19144902_trial_3.tiff
        but not 19144903_trial_1.tiff to 19144903_trial_3.tiff
        If user wants to extract the remaining 3 files from 19144903 camera, he/she has to call this function another time with different file structure, like

        extract_all_timestamp_from_folder('/data/', '19144903_trial_{}.tiff')

    @return: 
      This function return a dictionary with:
      key = (int) TRIAL_NUMBER -> value = (list) a list of all timestamp (in nano second) extracted in order from all image files of which name matches the given structure 
      if there are errors while extracting, this function return an empty dictionary
  """

  trial_dict = {}
  current_trial_index = first_trial_index
  # Normalize the folder path with no / at the end
  folder_path = folder_path.rstrip('/')
  # Synthesize full path
  full_path_to_tiff_file = f'{folder_path}/{file_name_structure}'
  while True:
    current_full_path_to_tiff_file = full_path_to_tiff_file.format(current_trial_index)
    if os.path.isfile(current_full_path_to_tiff_file):
      try:
        if verbose:
          print(f'Extracting data from file: {current_full_path_to_tiff_file}')
        trial_dict[current_trial_index] = extract_timestamp(current_full_path_to_tiff_file)
        current_trial_index += 1
      except Exception as e:
        print(f'Encountering error when extracting timestamp from {current_full_path_to_tiff_file} with error: {e}')
        return {}
    else:
      break
  return trial_dict