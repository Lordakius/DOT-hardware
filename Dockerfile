FROM python:3

WORKDIR /usr/src/app

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

RUN git clone https://github.com/adafruit/Adafruit_Python_PN532.git
RUN python Adafruit_Python_PN532/setup.py install

FROM python:3-alpine

COPY . .

CMD [ "python", "./src/tracker.py" ]
