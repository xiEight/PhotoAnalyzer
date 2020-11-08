from imageai.Detection import ObjectDetection
import os
import sys

execution_path = os.getcwd()
input_file = sys.argv[1]
output_file = sys.argv[2]
detector = ObjectDetection()
detector.setModelTypeAsRetinaNet()
detector.setModelPath( os.path.join(execution_path , "resnet50_coco_best_v2.0.1.h5"))
detector.loadModel()
detections = detector.detectObjectsFromImage(input_image=os.path.join(execution_path , input_file), output_image_path=os.path.join(execution_path , output_file), minimum_percentage_probability=30)

#for eachObject in detections:
#    print(eachObject["name"] , " : ", eachObject["percentage_probability"], " : ", eachObject["box_points"] )
#    print("--------------------------------")
#detections = detector.detectObjectsFromImage(input_image=os.path.join(execution_path , "image1.png"), output_image_path=os.path.join(execution_path , "image22new.png"), minimum_percentage_probability=30)
