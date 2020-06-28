# put this file inside the folder where your images are in
import PIL.Image as IMG
import tifffile as tf
import json

TAG_WITH_FRAME_ID_INFO = 'ImageDescription'
FRAME_ID_TAG_NAME = 'frameID'


def getFrameID(img):
    vl = img.tag[TAG_WITH_FRAME_ID_INFO]
    decodedJson = json.loads(vl)
    return decodedJson[FRAME_ID_TAG_NAME]


FileNameFormat = '{}_trial_{}_{}.tiff'		# CamSeries_trail_TrialNumber_PartNumber.tiff


# Get camera series
camSeries = input('What is the cam series?: ')

# Ask some question about file name format
TrialFirstIndex = int(input('Trail first index: '))
FilePartFirstIndex = int(input('File part first index: '))

# Number of files to analyze
NumFile = int(input('How many files to analyze? (0 means all):'))
if NumFile == 0:
    NumFile = -1

FileCounter = 0
TrialCounter = TrialFirstIndex
readErrorCount = 0
lastFrameId = -1
totalFrame = 0
missFrameCounter =0

# Run analysis
while FileCounter < NumFile:
    PartCounter = FilePartFirstIndex
    while True:
        try:
            tiff = tf.TiffFile(FileNameFormat.format(camSeries,TrialCounter,PartCounter))
            for p in range(len(tiff.pages)):
                img = tiff.pages[p]
                imgFrameID = getFrameID(img)
                if imgFrameID != lastFrameId + 1:
                    missFrameCounter += 1
                lastFrameId = imgFrameID
                totalFrame += 1
            PartCounter += 1
        except tf.TiffFileError:
            readErrorCount += 1
            break
        readErrorCount = 0
    if readErrorCount == 2:
        break
    else:
        TrialCounter += 1
print('Total acquired iamge: ',totalFrame)
print('Number of missed frame: ', missFrameCounter)
print('Estimate 1 missed frame every {} frame acquired'.format(totalFrame//missFrameCounter))