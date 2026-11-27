### Surgeon General's Warning

Some of the stuff in this document is extraneous, counter-productive, or
otherwise considered harmful for operative morale.

---

### `user-data`

edit some stuff near the bottom of `user-data`:
```
ssh_pwauth: false
rpi:
  enable_usb_gadget: true
  enable_ssh: true
runcmd:
  - [ systemctl, enable, --now, ssh ]
  - [ rfkill, unblock, wifi ]
  - [ sh, -c, "for f in /var/lib/systemd/rfkill/*:wlan; do echo 0 > \"$f\"; done" ]
```

the most important thing is that 
```
rpi:
  enable_usb_gadget: true
  enable_ssh: true
```
is right after the `ssh_pwauth: false` line
but before the `runcmd:` line.

**this has to be done before the first boot.**
it 100% won't do anything otherwise.

---

### `cmdline.txt`

insert `modules-load=dwc2,g_ether` into `cmdline.txt`, 
right after `rootwait`:
```
... rootwait modules-load=dwc2,g_ether ...
```

on the first boot, you may also need to add 
`systemd.mask=rpi-usb-gadget.service` to the end 
and then remove it afterwards.

**please remember that `cmdline.txt` has to be exactly one line.**

**ONE LINE. NO MORE, NO LESS.**

---

### `config.txt`

adjust `config.txt`:
```
# gotta add this near the top
enable_uart=1

...

# gotta comment this line out (near the middle)
#dtoverlay=vc4-kms-v3d

...

# and this has to be at the bottom
[all]
hdmi_force_hotplug=1
hdmi_safe=1
dtoverlay=dwc2,dr_mode=peripheral
```

