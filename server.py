from flask import Flask, request, jsonify
from PIL import Image
import io
import torch
import torchvision.transforms as transforms
from torchvision import models

app = Flask(__name__)

# Load the MobileNet model
model = models.mobilenet_v2(pretrained=True)
model.eval()

# Define the image transformation pipeline
transform = transforms.Compose([
    transforms.Resize((224, 224)),
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),
])

@app.route('/predict', methods=['POST'])
def predict():
    # Get the image from the POST request
    image_data = request.files['image'].read()
    image = Image.open(io.BytesIO(image_data))

    # Pre-process the image
    image = transform(image).unsqueeze(0)  # Add batch dimension

    # Run the model
    with torch.no_grad():
        outputs = model(image)

    # Post-process the outputs (e.g., get predicted class)
    _, predicted_class = outputs.max(1)
    return jsonify({"predicted_class": predicted_class.item()})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
