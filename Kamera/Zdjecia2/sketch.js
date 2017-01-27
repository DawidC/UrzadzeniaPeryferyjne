var video;
var button;
var snapshots = [];
var counter = 0;
var vScale = 4;
var total;

function setup() {
  createCanvas(800, 300);
  background(51);
  video = createCapture(VIDEO);
  video.size(320, 240);
  button = createButton('snap');
  button.mousePressed(takesnap);
 }

 function takesnap(){
 	snapshots.push(video.get());
 }

function draw() {
  var w = 80;
  var h = 60;
  var x = 0;
  var y = 0;
  for (var i = 0; i < snapshots.length; i++) {
    //tint(255, 50);
    image(snapshots[i], x, y, w, h);
    x = x + w;
    if (x >= width) {
      x = 0;
      y = y + h;
    }
  }
}


