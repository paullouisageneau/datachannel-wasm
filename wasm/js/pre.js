var canvasElement = document.getElementById('canvas');
var statusElement = document.getElementById('status');

Module['canvas'] = canvasElement;

Module['print'] = function(text) {
	console.log(text);
};
Module['printErr'] = function(text) {
    console.error(text);
},
Module['setStatus'] = function(text) {
    statusElement.innerHTML = text;
};

window.addEventListener('error', function() {
    Module.setStatus('Exception thrown, see JavaScript console');
    Module.setStatus = function() {};
});

document.addEventListener('keypress', function(evt) {
    if(evt.key == 'f') {
        Module.requestFullscreen(true, true);
    }
});

// As a default initial behavior, pop up an alert when webgl context is lost. To make your
// application robust, you may want to override this behavior before shipping!
// See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
canvasElement.addEventListener("webglcontextlost", function(e) {
    alert('WebGL context lost. You will need to reload the page.');
    e.preventDefault();
}, false);

