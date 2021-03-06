#import <Foundation/Foundation.h>
#import "MKMarkerDetector.h"
#import "MKEdgel.h"
#import "MKLineSegment.h"
#import "MKMarker.h"

static inline void MKDetectorClarkeConvert420VPixelBufferToMonochrome(CVPixelBufferRef pixelBuffer, unsigned char* monochromeBuffer) {

	uint8_t *baseAddress = (uint8_t *)CVPixelBufferGetBaseAddress(pixelBuffer);
	const size_t width = CVPixelBufferGetWidth(pixelBuffer);
	const size_t height = CVPixelBufferGetHeight(pixelBuffer);

	const unsigned int length = width * height;

	const unsigned int* offsets = (unsigned int*)baseAddress;
	unsigned int yOffset    = OSSwapInt32(offsets[0]);

	memcpy(monochromeBuffer, (baseAddress + yOffset), length);
}

@class AVFoundation;

@interface MKDetectorClarke : MKMarkerDetector {

	unsigned char* monochromeBuffer;
	edgelPool_t* edgels;
	linesegmentPool_t* linesegments;
	linesegmentPool_t* mergedLines;
	linesegmentPool_t* lineChains;
	markerPool_t* markers;
}

@property (nonatomic, assign) id <MKMarkerDetectorDelegate> delegate;
@property (nonatomic, assign) int imageWidth;
@property (nonatomic, assign) int imageHeight;
@property (nonatomic, assign, getter = isDrawingGrid) BOOL drawGrid;
@property (nonatomic, assign, getter = isDrawingEdgles) BOOL drawEdgels;
@property (nonatomic, assign, getter = isDrawingLineSegments) BOOL drawLineSegments;
@property (nonatomic, assign, getter = isDrawingMergedLines) BOOL drawMergedLines;
@property (nonatomic, assign, getter = isDrawingExtendedLines) BOOL drawExtendedLines;
@property (nonatomic, assign, getter = isDrawingMarkers) BOOL drawMarkers;

- (id)initWithDelegate:(id<MKMarkerDetectorDelegate>)aDelegate;
- (void)detectMarkerInImageBuffer:(CVImageBufferRef)imageBuffer;

@end

@interface MKDetectorClarke (Internal)

- (void)imageSizedChangedWidht:(int)width height:(int)height;

- (void)findEdgelsInFrame:(const unsigned char*)frame andCopyToEdgelPool:(edgelPool_t*)pool forRegionLeft:(const short)left andRegionTop:(const short)top;

- (void)detectLinesFromEdgels:(edgelPool_t *)edgelpool copyToLinePool:(linesegmentPool_t *)linesegmentPool;

- (void)mergeLinesInLinePool:(linesegmentPool_t *)pool;

- (void)extendLinesInLinePool:(linesegmentPool_t *)pool;

- (void)findLinesWithCornersInLinePool:(linesegmentPool_t *)pool copyToLinePool:(linesegmentPool_t *)linesWithCorners;

- (marker_t)reconstructCornersFromChains:(linesegmentPool_t*)chains;

- (void)findChainOfLinesInBuffer:(linesegmentPool_t*)searchBuffer withStartChain:(linesegment_t*)start fromStart:(BOOL)fromStart chainLength:(int*)chainLength copyToBuffer:(linesegmentPool_t*)chainBuffer;

vector_t MKDetectorClarkeGradientIntensity (const unsigned char* buffer, const unsigned int width, const unsigned int height, const int x, const int y);

int MKDetectorClarkeConvoluteKernelX (const unsigned char *buffer, short x, short y, const int width, const int height);

int MKDetectorClarkeConvoluteKernelY (const unsigned char *buffer, short x, short y, const int width, const int height);

bool MKDetectorClarkeExtendLine (vector_t start, const vector_t slope, const vector_t gradient, vector_t *end, const int maxLength, const unsigned char* buffer);

@end
