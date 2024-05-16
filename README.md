# ESP32 Lovense
This project allows an ESP32 to pretend to be a Lovense toy and receive Vibrate, Battery and DeviceType commands.  
I threw this together in a few hours, so it is very primitive and not at all modular.

The [buttplug.io](https://docs.buttplug.io/docs/stpihkal/protocols/lovense/) docs were very helpful while working on this, so definitely check that out if you plan on working with this code.

## Noteworthy Code
- [Line 5](https://github.com/JanoschABR/esp32-lovense/blob/912d554ca0b36be4284e523443da50acd4560352/firmware.ino#L5): `DEVICE_NAME` is the name this device will be available under. Note that the Lovense App does not respect this name, but instead determins the device name based on the Service and Characteristic UUIDs.
- [Line 29](https://github.com/JanoschABR/esp32-lovense/blob/912d554ca0b36be4284e523443da50acd4560352/firmware.ino#L29) / [Line 112](https://github.com/JanoschABR/esp32-lovense/blob/912d554ca0b36be4284e523443da50acd4560352/firmware.ino#L112): `lvsHandleMessage` is called whenever a BLE message has been received. It is given the incoming message as a string and returns a string as a response. Returning an empty string will result in no response being sent. If you want to implement additional Lovense commands, you'd put them inside `lvsHandleMessage`.
- [Line 115](https://github.com/JanoschABR/esp32-lovense/blob/912d554ca0b36be4284e523443da50acd4560352/firmware.ino#L115): The `Battery` command currently always returns 100%. If you have a device that actually has a battery, as well as a way to measure said battery's charge, you could implement battery monitoring by returning the proper value here. As a side note, the battery value doesn't seem to actually have any enforced range, so you can even have a charge level of 400% if you wanted to. I haven't tested negative numbers yet, but that might also work. Keep in mind that the Lovense Apps expect this to be a proper battery value, so changing it to anything not in the range of 0-100 might have some unforseen consequences.
- [Line 120](https://github.com/JanoschABR/esp32-lovense/blob/912d554ca0b36be4284e523443da50acd4560352/firmware.ino#L120): The `DeviceType` command currently returns an invalid type. The Lovense App doesn't seem to care, but it's definitely something worth looking into. Additionally, the model type, i.e. the first segment of the device type, doesn't seem to actually change anything relevant, because it seems the Lovense App detects the device name and model using the Service and Characteristic UUIDs.
- [Line 124](https://github.com/JanoschABR/esp32-lovense/blob/912d554ca0b36be4284e523443da50acd4560352/firmware.ino#L124): The `Vibrate` command is fully implemented, however since this project is purely software, nothing is actually done with the parsed data. The requested vibration strength will be stored in the `level` variable. The vibration strength is always in a range of 0-20, 20 being 100% intensity.

## TODOs & Future Plans
- [ ] Switch from Arduino IDE to native ESP-IDF with FreeRTOS (Should boost performance by quite a bit)
- [ ] Implement missing Lovense commands
- [ ] Reimplement entire system as an IDF component (i.e. as a library instead of hard-coded firmware)
- [ ] Figure out if there is a way to instruct the Lovense App to operate in higher resolution (instead of only 20 steps)
- [ ] Figure out how to spoof multi-vibrator devices, such as the Edge 2
- [ ] Figure out what the Service and Characteristic UUIDs of other models are and allow spoofing those too
- [ ] Add other control methods (Websocket? GUI?)
