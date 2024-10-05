let playback_buffer = new Array(), original_playback_buffer = new Array();

let undo_stack = new Array();
let redo_stack = new Array();

let stopNextTick = false;
let isRunning = false;
let frameCount = 0;
let playback_cursor = 0;
let fps = 0, fpsInterval = 0, startTime = 0, now = 0, then = 0, elapsed = 0, recordingLength = 0;

function resetBuffer() {
    playback_buffer = structuredClone(original_playback_buffer);
    playback_cursor = 0;
    update_ui(playback_cursor);
}

function save() {
    let serialized_buffer = new Int8Array(1 + playback_buffer.length * 16);
    let i = 0;
    serialized_buffer[i++] = recordingLength;
    playback_buffer.forEach(controllerData => {
        serialized_buffer[i++] = controllerData.axis[0];
        serialized_buffer[i++] = controllerData.axis[1];
        serialized_buffer[i++] = controllerData.axis[2];
        serialized_buffer[i++] = controllerData.axis[3];
        serialized_buffer[i++] = controllerData.digital[0];
        serialized_buffer[i++] = controllerData.digital[1];
        serialized_buffer[i++] = controllerData.digital[2];
        serialized_buffer[i++] = controllerData.digital[3];
        serialized_buffer[i++] = controllerData.digital[4];
        serialized_buffer[i++] = controllerData.digital[5];
        serialized_buffer[i++] = controllerData.digital[6];
        serialized_buffer[i++] = controllerData.digital[7];
        serialized_buffer[i++] = controllerData.digital[8];
        serialized_buffer[i++] = controllerData.digital[9];
        serialized_buffer[i++] = controllerData.digital[10];
        serialized_buffer[i++] = controllerData.digital[11];
    });

    let a = document.createElement("a");
    document.body.appendChild(a);
    a.style = "display: none";
    console.log(serialized_buffer)
    let blob = new Blob([serialized_buffer], {type: "octet/stream"});
    let url = window.URL.createObjectURL(blob);
    a.href = url;
    a.download = document.getElementById("input").files[0].name;
    a.click();
    window.URL.revokeObjectURL(url);
}

function add_cursor(idx) {
    playback_cursor += idx;
    update_ui(playback_cursor);
    $("#timeline").val(playback_cursor.toString());
}

function stop_playback() {
    if (isRunning) {
        stopNextTick = true;
    } else {
        update_ui(0);
        $("#timeline").val("0");
    }
}

function update_ui(idx) {
    $("#progress").val(idx.toString());

    $("#axis1").val(playback_buffer[idx].axis[0]);
    $("#axis2").val(playback_buffer[idx].axis[1]);
    $("#axis3").val(playback_buffer[idx].axis[2]);
    $("#axis4").val(playback_buffer[idx].axis[3]);

    $("#button_a").prop("checked", playback_buffer[idx].digital[0] == 1);
    $("#button_b").prop("checked", playback_buffer[idx].digital[1] == 1);
    $("#button_x").prop("checked", playback_buffer[idx].digital[2] == 1);
    $("#button_y").prop("checked", playback_buffer[idx].digital[3] == 1);
    $("#button_up").prop("checked", playback_buffer[idx].digital[4] == 1);
    $("#button_right").prop("checked", playback_buffer[idx].digital[5] == 1);
    $("#button_down").prop("checked", playback_buffer[idx].digital[6] == 1);
    $("#button_left").prop("checked", playback_buffer[idx].digital[7] == 1);
    $("#button_l1").prop("checked", playback_buffer[idx].digital[8] == 1);
    $("#button_l2").prop("checked", playback_buffer[idx].digital[9] == 1);
    $("#button_r1").prop("checked", playback_buffer[idx].digital[10] == 1);
    $("#button_r2").prop("checked", playback_buffer[idx].digital[11] == 1);

    $("#digital_a").toggleClass("pressing", playback_buffer[idx].digital[0] == 1);
    $("#digital_b").toggleClass("pressing", playback_buffer[idx].digital[1] == 1);
    $("#digital_x").toggleClass("pressing", playback_buffer[idx].digital[2] == 1);
    $("#digital_y").toggleClass("pressing", playback_buffer[idx].digital[3] == 1);
    $("#digital_up").toggleClass("pressing", playback_buffer[idx].digital[4] == 1);
    $("#digital_right").toggleClass("pressing", playback_buffer[idx].digital[5] == 1);
    $("#digital_down").toggleClass("pressing", playback_buffer[idx].digital[6] == 1);
    $("#digital_left").toggleClass("pressing", playback_buffer[idx].digital[7] == 1);
    $("#digital_l1").toggleClass("pressing", playback_buffer[idx].digital[8] == 1);
    $("#digital_l2").toggleClass("pressing", playback_buffer[idx].digital[9] == 1);
    $("#digital_r1").toggleClass("pressing", playback_buffer[idx].digital[10] == 1);
    $("#digital_r2").toggleClass("pressing", playback_buffer[idx].digital[11] == 1);
    
    $("#joycon_right").css("transform", "translate(" + (playback_buffer[idx].axis[0] * 2.5) + "px," + (playback_buffer[idx].axis[1] * -2.5) + "px)")
    $("#joycon_left").css("transform", "translate(" + (playback_buffer[idx].axis[2] * 2.5) + "px," + (playback_buffer[idx].axis[3] * -2.5) + "px)")
}

function begin_playback(start_pos = 0) {
    if (isRunning) return;
    if (playback_cursor == playback_buffer.length - 1) start_pos = 0;
    fpsInterval = 1000 / 60;
    playback_cursor = start_pos;
    then = window.performance.now();
    startTime = then;
    console.log(startTime);
    tickPlayback();
    $("#button_stop").text("Pause");
    isRunning = true;
}

function tickPlayback() {
    // stop if user says to stop or if cursor has reached end
    if (stopNextTick || playback_cursor >= playback_buffer.length - 1) {
        stopNextTick = false;
        isRunning = false;
        $("#button_stop").text("Stop");
        return;
    }

    // request another frame
    requestAnimationFrame(tickPlayback);

    // calc (slang for calculate) elapsed time since last loop
    now = window.performance.now();
    elapsed = now - then;
            
    // if enough time has elapsed, draw the next frame
    if (elapsed > fpsInterval) {
        // Get ready for next frame by setting then=now
        then = now - (elapsed % fpsInterval);

        // update stuff
        playback_cursor++;
        update_ui(playback_cursor);
        $("#timeline").val(playback_cursor.toString());
    }
}

function saveToBuffer(section, idx, data) {
    playback_buffer[playback_cursor][section][idx] = data;
    update_ui(playback_cursor);
}

window.onload = function () {
    let input = document.getElementById("input");
    input.addEventListener("change", () => {
        // clear out the array in the event of loading a 
        playback_buffer.length = 0;
        let reader = new FileReader();
        reader.onload = function () {
            // read out the entire file (kinda like a C io stream but manually implemented)
            let arrayBuffer = this.result, array = new Int8Array(arrayBuffer);
            let cursor = 0;
            let length = array[cursor++];
            recordingLength = length;
            while (cursor < array.byteLength) {
                playback_buffer.push({
                    "axis": [array[cursor++], array[cursor++], array[cursor++], array[cursor++]],
                    "digital": [array[cursor++], array[cursor++], array[cursor++], array[cursor++], array[cursor++], array[cursor++], array[cursor++], array[cursor++], array[cursor++], array[cursor++], array[cursor++], array[cursor++]]
                });
            }
            // copy the original to a reserve buffer
            original_playback_buffer = structuredClone(playback_buffer);

            // Make all the UI
            console.log("Data says length is " + length + " seconds.");
            $("#controller_ui").removeClass("hidden");
            $("#data_ui").removeClass("hidden");
            $("#timeline").css("width", (length * 100) + "px");
            $("#timeline").attr("max", playback_buffer.length - 1);
            $("#progress").val("0");
            $("#max").text("/" + (playback_buffer.length - 1).toString());
            document.getElementById("timeline").oninput = function (event) {
                playback_cursor = Number(event.target.value);
                update_ui(playback_cursor);
            }
        }
        reader.readAsArrayBuffer(input.files[0]);
    });

    document.getElementById("axis1")        .addEventListener("change", e => { saveToBuffer("axis", 0, e.target.value) });
    document.getElementById("axis2")        .addEventListener("change", e => { saveToBuffer("axis", 1, e.target.value) });
    document.getElementById("axis3")        .addEventListener("change", e => { saveToBuffer("axis", 2, e.target.value) });
    document.getElementById("axis4")        .addEventListener("change", e => { saveToBuffer("axis", 3, e.target.value) });
    document.getElementById("button_a")     .addEventListener("change", e => { saveToBuffer("digital", 0, e.target.value) });
    document.getElementById("button_b")     .addEventListener("change", e => { saveToBuffer("digital", 1, e.target.value) });
    document.getElementById("button_x")     .addEventListener("change", e => { saveToBuffer("digital", 2, e.target.value) });
    document.getElementById("button_y")     .addEventListener("change", e => { saveToBuffer("digital", 3, e.target.value) });
    document.getElementById("button_up")    .addEventListener("change", e => { saveToBuffer("digital", 4, e.target.value) });
    document.getElementById("button_right") .addEventListener("change", e => { saveToBuffer("digital", 5, e.target.value) });
    document.getElementById("button_down")  .addEventListener("change", e => { saveToBuffer("digital", 6, e.target.value) });
    document.getElementById("button_left")  .addEventListener("change", e => { saveToBuffer("digital", 7, e.target.value) });
    document.getElementById("button_l1")    .addEventListener("change", e => { saveToBuffer("digital", 8, e.target.value) });
    document.getElementById("button_l2")    .addEventListener("change", e => { saveToBuffer("digital", 9, e.target.value) });
    document.getElementById("button_r1")    .addEventListener("change", e => { saveToBuffer("digital", 10, e.target.value) });
    document.getElementById("button_r2")    .addEventListener("change", e => { saveToBuffer("digital", 11, e.target.value) });
}

window.onkeydown = function (ev) {
    if (ev.ctrlKey && ev.key == 'z') {
        if (ev.shiftKey) {} // redo
        //TODO: implement undo/redo
    }
}
