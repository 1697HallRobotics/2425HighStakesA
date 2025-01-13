const {Tween, Easing, Group} = TWEEN;

let playback_buffer = new Array(), original_playback_buffer = new Array();

let undo_stack = new Array();
let redo_stack = new Array();

let tweens = new Array();

let stopNextTick = false;
let isRunning = false;
let frameCount = 0;
let playback_cursor = 0;
let previous_position = 0;
let fps = 0, fpsInterval = 0, startTime = 0, now = 0, then = 0, elapsed = 0, recordingLength = 0;

let coords = {x: -50, y: -50};
let alpha = {val: 0};

function duplicate_action() {
    let amount = Number(document.getElementById("duplication_captures").value);
    let recent_action = undo_stack[undo_stack.length - 1];
    if (recent_action.action != "update_buffer") return;
    for (let i = 1; i <= amount; i++) {
        playback_buffer[playback_cursor + i][recent_action.common_data[0]][recent_action.common_data[1]] = recent_action.new_data[0]
    };

    update_ui(playback_cursor);
};

function add_undo_action(action) {
    undo_stack.push(action);
    redo_stack.length = 0;
};

function undo() {
    if (undo_stack.length == 0) return;
    let undo_action = undo_stack.pop();

    switch (undo_action.action) {
        case "update_buffer":
            playback_cursor = undo_action.cursor_position;
            playback_buffer[playback_cursor][undo_action.common_data[0]][undo_action.common_data[1]] = undo_action.old_data[0];
            break;
        case "move_cursor":
            playback_cursor = undo_action.old_data[0];
            break;
        default:
            break;
    }

    redo_stack.push(undo_action);

    update_ui(playback_cursor);
};

function redo() {
    if (redo_stack.length == 0) return;
    let redo_action = redo_stack.pop();

    switch (redo_action.action) {
        case "update_buffer":
            playback_cursor = redo_action.cursor_position;
            playback_buffer[playback_cursor][redo_action.common_data[0]][redo_action.common_data[1]] = redo_action.new_data[0];
            break;
        case "move_cursor":
            playback_cursor = redo_action.new_data[0];
            break;
        default:
            break;
    };

    undo_stack.push(redo_action);

    update_ui(playback_cursor);
};

function resetBuffer() {
    playback_buffer = structuredClone(original_playback_buffer);
    playback_cursor = 0;
    update_ui(playback_cursor);
};

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
    
    let blob = new Blob([serialized_buffer], {type: "octet/stream"});
    let url = window.URL.createObjectURL(blob);

    a.href = url;
    a.download = document.getElementById("input").files[0].name;
    a.click();

    window.URL.revokeObjectURL(url);
};

function add_cursor(idx) {
    add_undo_action({
        "action": "move_cursor",
        "common_data": [],
        "old_data": [Number(playback_cursor)],
        "new_data": [Number(playback_cursor) + Number(idx)],
        "cursor_position": playback_cursor
    });
    
    playback_cursor += idx;
    update_ui(playback_cursor);
};

function stop_playback() {
    if (isRunning) stopNextTick = true;
    else update_ui(0);
};

function update_ui(idx) {
    $("#progress").val(idx.toString());
    $("#timeline").val(idx.toString());

    $("#axis1").val(playback_buffer[idx].axis[0]);
    $("#axis2").val(playback_buffer[idx].axis[1]);
    $("#axis3").val(playback_buffer[idx].axis[2]);
    $("#axis4").val(playback_buffer[idx].axis[3]);

    $("#button_a")    .prop("checked", playback_buffer[idx].digital[0] == 1);
    $("#button_b")    .prop("checked", playback_buffer[idx].digital[1] == 1);
    $("#button_x")    .prop("checked", playback_buffer[idx].digital[2] == 1);
    $("#button_y")    .prop("checked", playback_buffer[idx].digital[3] == 1);
    $("#button_up")   .prop("checked", playback_buffer[idx].digital[4] == 1);
    $("#button_right").prop("checked", playback_buffer[idx].digital[5] == 1);
    $("#button_down") .prop("checked", playback_buffer[idx].digital[6] == 1);
    $("#button_left") .prop("checked", playback_buffer[idx].digital[7] == 1);
    $("#button_l1")   .prop("checked", playback_buffer[idx].digital[8] == 1);
    $("#button_l2")   .prop("checked", playback_buffer[idx].digital[9] == 1);
    $("#button_r1")   .prop("checked", playback_buffer[idx].digital[10] == 1);
    $("#button_r2")   .prop("checked", playback_buffer[idx].digital[11] == 1);

    $("#digital_a")    .toggleClass("pressing", playback_buffer[idx].digital[0] == 1);
    $("#digital_b")    .toggleClass("pressing", playback_buffer[idx].digital[1] == 1);
    $("#digital_x")    .toggleClass("pressing", playback_buffer[idx].digital[2] == 1);
    $("#digital_y")    .toggleClass("pressing", playback_buffer[idx].digital[3] == 1);
    $("#digital_up")   .toggleClass("pressing", playback_buffer[idx].digital[4] == 1);
    $("#digital_right").toggleClass("pressing", playback_buffer[idx].digital[5] == 1);
    $("#digital_down") .toggleClass("pressing", playback_buffer[idx].digital[6] == 1);
    $("#digital_left") .toggleClass("pressing", playback_buffer[idx].digital[7] == 1);
    $("#digital_l1")   .toggleClass("pressing", playback_buffer[idx].digital[8] == 1);
    $("#digital_l2")   .toggleClass("pressing", playback_buffer[idx].digital[9] == 1);
    $("#digital_r1")   .toggleClass("pressing", playback_buffer[idx].digital[10] == 1);
    $("#digital_r2")   .toggleClass("pressing", playback_buffer[idx].digital[11] == 1);
    
    $("#joycon_right").css("transform", "translate(" + (playback_buffer[idx].axis[0] * 2.5) + "px," + (playback_buffer[idx].axis[1] * -2.5) + "px)");
    $("#joycon_left") .css("transform", "translate(" + (playback_buffer[idx].axis[3] * 2.5) + "px," + (playback_buffer[idx].axis[2] * -2.5) + "px)");

    $("#undostack").text(JSON.stringify(undo_stack));
    $("#redostack").text(JSON.stringify(redo_stack));

    Array.from(document.getElementsByTagName("input")).forEach(item => {
        item.blur();
    });
};

function begin_playback(start_pos = 0) {
    if (isRunning) return;
    if (playback_cursor == playback_buffer.length - 1) start_pos = 0;
    fpsInterval = 15;
    playback_cursor = start_pos;
    then = window.performance.now();
    startTime = then;
    console.log(startTime);
    tickPlayback();
    $("#button_stop").text("Pause");
    isRunning = true;
};

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
    };
};

function saveToBuffer(section, idx, data) {
    let newValue = section === "axis" ? data : (data ? 1 : 0);
    add_undo_action({
        "action": "update_buffer",
        "common_data": [section, idx],
        "old_data": [playback_buffer[playback_cursor][section][idx]],
        "new_data": [newValue],
        "cursor_position": playback_cursor
    });
    playback_buffer[playback_cursor][section][idx] = newValue;
    
    update_ui(playback_cursor);
}

function animate(time) {
    tweens.forEach((val) => {
        val.update(time)
        if (!val.isPlaying()) {
            const index = tweens.indexOf(val);
            if (index > -1) { // only splice array when item is found
                tweens.splice(index, 1); // 2nd parameter means remove one item only
            }
        }
    })
    requestAnimationFrame(animate);
}

window.onload = function () {
    let input = document.getElementById("input");
    input.addEventListener("change", () => {
        // clear out the array in the event of loading a new file if one is already loaded
        if (coords.y == -50) {
            const tween = new Tween(coords, false).to({x: -50, y: 400 }, 1000).easing(Easing.Quadratic.InOut).onUpdate(() => {
                document.getElementById("input_button").style.setProperty("transform", "translate(" + coords.x + "%, " + coords.y + "%)");
            }).start();
            const tween2 = new Tween(alpha, false).to({val: 100}, 1200).easing(Easing.Quadratic.InOut).onUpdate(() => {
                document.getElementById("data_ui").style.setProperty("opacity", alpha.val + "%");
                document.getElementById("controller_ui").style.setProperty("opacity", alpha.val + "%");
            }).start();
            tweens.push(tween);
            tweens.push(tween2);
        }
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
            };
            // copy the original to a reserve buffer
            original_playback_buffer = structuredClone(playback_buffer);

            // Make all the UI
            console.log("Data says length is " + length + " seconds.");
            $("#controller_ui").removeClass("hidden");
            $("#data_ui").removeClass("hidden");
            $("#timeline").css("width", (length * 40) + "px");
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

    document.getElementById("axis1")        .addEventListener("change", e => { saveToBuffer("axis",     0, e.target.value); e.target.blur() });
    document.getElementById("axis2")        .addEventListener("change", e => { saveToBuffer("axis",     1, e.target.value); e.target.blur() });
    document.getElementById("axis3")        .addEventListener("change", e => { saveToBuffer("axis",     2, e.target.value); e.target.blur() });
    document.getElementById("axis4")        .addEventListener("change", e => { saveToBuffer("axis",     3, e.target.value); e.target.blur() });
    document.getElementById("button_a")     .addEventListener("change", e => { saveToBuffer("digital",  0, e.target.checked); e.target.blur() });
    document.getElementById("button_b")     .addEventListener("change", e => { saveToBuffer("digital",  1, e.target.checked); e.target.blur() });
    document.getElementById("button_x")     .addEventListener("change", e => { saveToBuffer("digital",  2, e.target.checked); e.target.blur() });
    document.getElementById("button_y")     .addEventListener("change", e => { saveToBuffer("digital",  3, e.target.checked); e.target.blur() });
    document.getElementById("button_up")    .addEventListener("change", e => { saveToBuffer("digital",  4, e.target.checked); e.target.blur() });
    document.getElementById("button_right") .addEventListener("change", e => { saveToBuffer("digital",  5, e.target.checked); e.target.blur() });
    document.getElementById("button_down")  .addEventListener("change", e => { saveToBuffer("digital",  6, e.target.checked); e.target.blur() });
    document.getElementById("button_left")  .addEventListener("change", e => { saveToBuffer("digital",  7, e.target.checked); e.target.blur() });
    document.getElementById("button_l1")    .addEventListener("change", e => { saveToBuffer("digital",  8, e.target.checked); e.target.blur() });
    document.getElementById("button_l2")    .addEventListener("change", e => { saveToBuffer("digital",  9, e.target.checked); e.target.blur() });
    document.getElementById("button_r1")    .addEventListener("change", e => { saveToBuffer("digital", 10, e.target.checked); e.target.blur() });
    document.getElementById("button_r2")    .addEventListener("change", e => { saveToBuffer("digital", 11, e.target.checked); e.target.blur() });
    
    document.getElementById("timeline")     .addEventListener("mouseup", e => {
        e.preventDefault();
        add_undo_action({
            "action": "move_cursor",
            "common_data": [],
            "old_data": [Number(previous_position)],
            "new_data": [Number(playback_cursor)],
            "cursor_position": playback_cursor
        })
        previous_position = playback_cursor;
    });

    requestAnimationFrame(animate);
}

window.onkeydown = function (evt) {
    if ((evt.key === 'Z' && (evt.ctrlKey || evt.metaKey) && evt.shiftKey) || (evt.key === 'y' && (evt.ctrlKey || evt.metaKey))) {
        evt.preventDefault();
        // handle redo action
        redo();
    } else if (evt.key === 'z' && (evt.ctrlKey || evt.metaKey)) {
        evt.preventDefault();
        // handle undo action
        undo();
    }
}