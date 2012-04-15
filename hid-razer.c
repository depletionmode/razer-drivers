#include <linux/device.h>
#include <linux/hid.h>
#include <linux/module.h>

#define DRIVER_AUTHOR "David Kaplan, david@2of1.org"
#define DRIVER_DESC "Razer input device driver"

#define TOGGLE_KEY(X,Y) input_report_key(X,Y,1); \
                        input_report_key(X,Y,0)
enum type {
    TYPE_NAGA,
    TYPE_COUNT
};

struct naga_data {
    int keymap[12];
};

struct razer_data {
    struct usb_device *udev;
    enum type type;
    union {
        struct naga_data naga_data;
    };
};

#define SHOW_STORE_KEY(value)	                              \
static ssize_t                                                \
show_key##value(struct device *dev,                           \
                struct device_attribute *attr, char *buf)     \
{                                                             \
    struct hid_device *hdev = container_of(dev,               \
                                           struct hid_device, \
                                           dev);              \
    struct razer_data *data = hid_get_drvdata(hdev);          \
                                                              \
    if (data->type == TYPE_NAGA)                              \
        return sprintf(buf, "%d\n",                           \
                       data->naga_data.keymap[value-1]);      \
    return 0;                                                 \
}                                                             \
                                                              \
static ssize_t                                                \
store_key##value(struct device *dev,                          \
                 struct device_attribute *attr,               \
                 const char *buf,                             \
                 size_t count)                                \
{                                                             \
    struct hid_device *hdev = container_of(dev,               \
                                           struct hid_device, \
                                           dev);              \
    struct razer_data *data = hid_get_drvdata(hdev);          \
                                                              \
    if (data->type == TYPE_NAGA) {                            \
        int temp = simple_strtoul(buf, NULL, 10);             \
        data->naga_data.keymap[value-1] = temp;               \
    }                                                         \
   return count;                                              \
}                                                             \
static DEVICE_ATTR(key##value,                                \
                   S_IWUGO | S_IRUGO,                         \
                   show_key##value,                           \
                   store_key##value);                         \

SHOW_STORE_KEY(1)
SHOW_STORE_KEY(2)
SHOW_STORE_KEY(3)
SHOW_STORE_KEY(4)
SHOW_STORE_KEY(5)
SHOW_STORE_KEY(6)
SHOW_STORE_KEY(7)
SHOW_STORE_KEY(8)
SHOW_STORE_KEY(9)
SHOW_STORE_KEY(10)
SHOW_STORE_KEY(11)
SHOW_STORE_KEY(12)

static const struct hid_device_id razer_devices[] = {
    { HID_USB_DEVICE(0x1532, 0x0015),
          .driver_data = TYPE_NAGA },
    { }
};
MODULE_DEVICE_TABLE(hid, razer_devices);

static int razer_probe(struct hid_device *hdev,
                       const struct hid_device_id *id)
{
    int ret;
    struct razer_data *data;

    ret = hid_parse(hdev);

    data = kzalloc(sizeof(struct razer_data), GFP_KERNEL);

    data->type = id->driver_data;

    if (hdev->type == HID_TYPE_OTHER && hdev->product == 0x0015 /* Naga */) {
        hid_info(hdev, "Razer Naga device detected, extensions enabled");

        data->naga_data.keymap[0] = KEY_1;
        data->naga_data.keymap[1] = KEY_2;
        data->naga_data.keymap[2] = KEY_3;
        data->naga_data.keymap[3] = KEY_4;
        data->naga_data.keymap[4] = KEY_5;
        data->naga_data.keymap[5] = KEY_6;
        data->naga_data.keymap[6] = KEY_7;
        data->naga_data.keymap[7] = KEY_8;
        data->naga_data.keymap[8] = KEY_9;
        data->naga_data.keymap[9] = KEY_0;
        data->naga_data.keymap[10] = KEY_MINUS;
        data->naga_data.keymap[11] = KEY_EQUAL;
    
        device_create_file(&hdev->dev, &dev_attr_key1);
        device_create_file(&hdev->dev, &dev_attr_key2);
        device_create_file(&hdev->dev, &dev_attr_key3);
        device_create_file(&hdev->dev, &dev_attr_key4);
        device_create_file(&hdev->dev, &dev_attr_key5);
        device_create_file(&hdev->dev, &dev_attr_key6);
        device_create_file(&hdev->dev, &dev_attr_key7);
        device_create_file(&hdev->dev, &dev_attr_key8);
        device_create_file(&hdev->dev, &dev_attr_key9);
        device_create_file(&hdev->dev, &dev_attr_key10);
        device_create_file(&hdev->dev, &dev_attr_key11);
        device_create_file(&hdev->dev, &dev_attr_key12);
    }

    hid_set_drvdata(hdev, data);

    ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);

    return ret;
}

static int razer_event(struct hid_device *hdev, struct hid_field *field,
                       struct hid_usage *usage, __s32 value)
{
    if (usage->type == EV_KEY && value && hdev->type == HID_TYPE_OTHER) {
        struct input_dev *input = field->hidinput->input;
        struct razer_data *data = hid_get_drvdata(hdev);

        if (data->type == TYPE_NAGA) {
            TOGGLE_KEY(input, data->naga_data.keymap[usage->code-2]);
            return -1;
        }
    }

    return 0;
}

static void razer_remove(struct hid_device *hdev)
{
    struct razer_data *data = hid_get_drvdata(hdev);

    if (hdev->product == 0x0015 /* Naga */) {
        device_remove_file(&hdev->dev, &dev_attr_key1);
        device_remove_file(&hdev->dev, &dev_attr_key2);
        device_remove_file(&hdev->dev, &dev_attr_key3);
        device_remove_file(&hdev->dev, &dev_attr_key4);
        device_remove_file(&hdev->dev, &dev_attr_key5);
        device_remove_file(&hdev->dev, &dev_attr_key6);
        device_remove_file(&hdev->dev, &dev_attr_key7);
        device_remove_file(&hdev->dev, &dev_attr_key8);
        device_remove_file(&hdev->dev, &dev_attr_key9);
        device_remove_file(&hdev->dev, &dev_attr_key10);
        device_remove_file(&hdev->dev, &dev_attr_key11);
        device_remove_file(&hdev->dev, &dev_attr_key12);
    }

    kfree(data);
}

static struct hid_driver razer_driver = {
    .name = "razer",
    .id_table = razer_devices,
    .probe = razer_probe,
    .event = razer_event,
    .remove = razer_remove,
};

static int __init razer_init(void)
{
    return hid_register_driver(&razer_driver);
}

static void __exit razer_exit(void)
{
    hid_unregister_driver(&razer_driver);
}

module_init(razer_init);
module_exit(razer_exit);
MODULE_LICENSE("GPL");

