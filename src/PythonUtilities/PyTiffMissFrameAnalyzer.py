import PIL.Image as IMG
import tifffile as tf
import json

TAG_WITH_FRAME_ID_INFO = 'ImageDescription'
FRAME_ID_TAG_NAME = 'frameID'


def getFrameID(img):
    vl = img.tags[TAG_WITH_FRAME_ID_INFO].value
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
missFrameCounter = 0
currentFileMissedCounter = 0

# Run analysis
while FileCounter != NumFile:
    PartCounter = FilePartFirstIndex
    while True:
        try:
            fileName = FileNameFormat.format(camSeries,TrialCounter,PartCounter)
            tiff = tf.TiffFile(fileName)
            FileCounter += 1
            for p in range(len(tiff.pages)):
                img = tiff.pages[p]
                imgFrameID = getFrameID(img)
                if imgFrameID != lastFrameId + 1:
                    missedDist = imgFrameID - lastFrameId - 1
                    currentFileMissedCounter += missedDist
                    missFrameCounter += missedDist
                lastFrameId = imgFrameID
            print('Running analysis on file',fileName)
            print('Missed frame found: ',missFrameCounter)
            PartCounter += 1
            tiff.close()
        except FileNotFoundError:
            readErrorCount += 1
            break
        currentFileMissedCounter = 0
        readErrorCount = 0
    if readErrorCount == 2:
        break
    else:
        TrialCounter += 1
totalFrame = lastFrameId + 1
print('Total acquired image: ',totalFrame)
print('Number of missed frame: ', missFrameCounter)
if missFrameCounter != 0:
    print('Estimate 1 missed frame every {} frame acquired'.format(totalFrame/missFrameCounter))