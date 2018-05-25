#include "dslr.h"
#include "ui_dslr.h"

//#include <unistd.h>
//#include <stdlib.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
//#include <stdio.h>
//#include <stdarg.h>
//#include <string.h>
#include "globalsettings.h"



DSLR::DSLR(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DSLR)
{
    ui->setupUi(this);
    enabled=false;
    QPixmap image("media/icons/tools-32x32/led-red.png");
    ui->ledLabel->setPixmap(image);
}

DSLR::~DSLR()
{
    delete ui;
}

void DSLR::on_closeButton_clicked()
{
    close();
}

void DSLR::on_enableButton_clicked()
{
    if(enabled)
    {
        enabled=false;
        QPixmap image("media/icons/tools-32x32/led-red.png");
        ui->ledLabel->setPixmap(image);

    }
    else
    {
        enabled=true;
        QPixmap image("media/icons/tools-32x32/led-green.png");
        ui->ledLabel->setPixmap(image);
    }
}




int DSLR::_lookup_widget(CameraWidget*widget, const char *key, CameraWidget **child) {
    int ret;
    ret = gp_widget_get_child_by_name (widget, key, child);
    if (ret < GP_OK)
        ret = gp_widget_get_child_by_label (widget, key, child);
    return ret;
}

// calls the Nikon DSLR or Canon DSLR autofocus method.
int DSLR::camera_eosviewfinder(Camera *camera, GPContext *context, int onoff) {
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret,val;

    ret = gp_camera_get_config (camera, &widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }
    ret = _lookup_widget (widget, "eosviewfinder", &child);
    if (ret < GP_OK) {
        fprintf (stderr, "lookup 'eosviewfinder' failed: %d\n", ret);
        goto out;
    }

    // check that this is a toggle
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK) {
        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type) {
        case GP_WIDGET_TOGGLE:
        break;
    default:
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }

    ret = gp_widget_get_value (child, &val);
    if (ret < GP_OK) {
        fprintf (stderr, "could not get widget value: %d\n", ret);
        goto out;
    }
    val = onoff;
    ret = gp_widget_set_value (child, &val);
    if (ret < GP_OK) {
        fprintf (stderr, "could not set widget value to 1: %d\n", ret);
        goto out;
    }

    ret = gp_camera_set_config (camera, widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "could not set config tree to eosviewfinder: %d\n", ret);
        goto out;
    }
out:
    gp_widget_free (widget);
    return ret;
}

int DSLR::camera_auto_focus(Camera *camera, GPContext *context, int onoff) {
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret,val;

    ret = gp_camera_get_config (camera, &widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }
    ret = _lookup_widget (widget, "autofocusdrive", &child);
    if (ret < GP_OK) {
        fprintf (stderr, "lookup 'autofocusdrive' failed: %d\n", ret);
        goto out;
    }

    // check that this is a toggle
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK) {
        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type) {
        case GP_WIDGET_TOGGLE:
        break;
    default:
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }

    ret = gp_widget_get_value (child, &val);
    if (ret < GP_OK) {
        fprintf (stderr, "could not get widget value: %d\n", ret);
        goto out;
    }

    val = onoff;

    ret = gp_widget_set_value (child, &val);
    if (ret < GP_OK) {
        fprintf (stderr, "could not set widget value to 1: %d\n", ret);
        goto out;
    }

    ret = gp_camera_set_config (camera, widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "could not set config tree to autofocus: %d\n", ret);
        goto out;
    }
out:
    gp_widget_free (widget);
    return ret;
}


// Manual focusing a camera...
// xx is -3 / -2 / -1 / 0 / 1 / 2 / 3
//
int DSLR::camera_manual_focus (Camera *camera, int xx, GPContext *context) {
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret;
    float			rval;
    char			*mval;

    ret = gp_camera_get_config (camera, &widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }
    ret = _lookup_widget (widget, "manualfocusdrive", &child);
    if (ret < GP_OK) {
        fprintf (stderr, "lookup 'manualfocusdrive' failed: %d\n", ret);
        goto out;
    }

    // check that this is a toggle
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK) {
        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type) {
        case GP_WIDGET_RADIO: {
        int choices = gp_widget_count_choices (child);

        ret = gp_widget_get_value (child, &mval);
        if (ret < GP_OK) {
            fprintf (stderr, "could not get widget value: %d\n", ret);
            goto out;
        }
        if (choices == 7) { // see what Canon has in EOS_MFDrive
            ret = gp_widget_get_choice (child, xx+4, (const char**)&mval);
            if (ret < GP_OK) {
                fprintf (stderr, "could not get widget choice %d: %d\n", xx+2, ret);
                goto out;
            }
            fprintf(stderr,"manual focus %d -> %s\n", xx, mval);
        }
        ret = gp_widget_set_value (child, mval);
        if (ret < GP_OK) {
            fprintf (stderr, "could not set widget value to 1: %d\n", ret);
            goto out;
        }
        break;
    }
        case GP_WIDGET_RANGE:
        ret = gp_widget_get_value (child, &rval);
        if (ret < GP_OK) {
            fprintf (stderr, "could not get widget value: %d\n", ret);
            goto out;
        }

        switch (xx) { // Range is on Nikon from -32768 <-> 32768
        case -3:	rval = -1024;break;
        case -2:	rval =  -512;break;
        case -1:	rval =  -128;break;
        case  0:	rval =     0;break;
        case  1:	rval =   128;break;
        case  2:	rval =   512;break;
        case  3:	rval =  1024;break;

        default:	rval = xx;	break; // hack
        }

        fprintf(stderr,"manual focus %d -> %f\n", xx, rval);

        ret = gp_widget_set_value (child, &rval);
        if (ret < GP_OK) {
            fprintf (stderr, "could not set widget value to 1: %d\n", ret);
            goto out;
        }
        break;
    default:
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }


    ret = gp_camera_set_config (camera, widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "could not set config tree to autofocus: %d\n", ret);
        goto out;
    }
out:
    gp_widget_free (widget);
    return ret;
}







static int _lookup_widget(CameraWidget*widget, const char *key, CameraWidget **child) {
    int ret;
    ret = gp_widget_get_child_by_name (widget, key, child);
    if (ret < GP_OK)
        ret = gp_widget_get_child_by_label (widget, key, child);
    return ret;
}



// Gets a string configuration value.
//  This can be:
//   - A Text widget
//   - The current selection of a Radio Button choice
//   - The current selection of a Menu choice
//
//  Sample (for Canons eg):
//    get_config_value_string (camera, "owner", &ownerstr, context);
//
int DSLR::get_config_value_string (Camera *camera, const char *key, char **str, GPContext *context) {
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret;
    char			*val;

    ret = gp_camera_get_single_config (camera, key, &child, context);
    if (ret == GP_OK) {
        if (!child) fprintf(stderr,"child is NULL?\n");
        widget = child;
    } else {
        ret = gp_camera_get_config (camera, &widget, context);
        if (ret < GP_OK) {
            fprintf (stderr, "camera_get_config failed: %d\n", ret);
            return ret;
        }
        ret = _lookup_widget (widget, key, &child);
        if (ret < GP_OK) {
            fprintf (stderr, "lookup widget failed: %d\n", ret);
            goto out;
        }
    }

    // This type check is optional, if you know what type the label
    //  has already. If you are not sure, better check.
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK) {
        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type) {
        case GP_WIDGET_MENU:
        case GP_WIDGET_RADIO:
        case GP_WIDGET_TEXT:
        break;
    default:
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }

    // This is the actual query call. Note that we just
    // a pointer reference to the string, not a copy...
    ret = gp_widget_get_value (child, &val);
    if (ret < GP_OK) {
        fprintf (stderr, "could not query widget value: %d\n", ret);
        goto out;
    }
    // Create a new copy for our caller.
    *str = strdup (val);
out:
    gp_widget_free (widget);
    return ret;
}


// Sets a string configuration value.
//  This can set for:
//   - A Text widget
//   - The current selection of a Radio Button choice
//   - The current selection of a Menu choice
//
//  Sample (for Canons eg):
//    get_config_value_string (camera, "owner", &ownerstr, context);
//
int DSLR::set_config_value_string (Camera *camera, const char *key, const char *val, GPContext *context) {
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret;

    ret = gp_camera_get_config (camera, &widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }
    ret = _lookup_widget (widget, key, &child);
    if (ret < GP_OK) {
        fprintf (stderr, "lookup widget failed: %d\n", ret);
        goto out;
    }

    // This type check is optional, if you know what type the label
    //  has already. If you are not sure, better check.
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK) {
        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type) {
        case GP_WIDGET_MENU:
        case GP_WIDGET_RADIO:
        case GP_WIDGET_TEXT:
        break;
    default:
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }

    // This is the actual set call. Note that we keep
    // ownership of the string and have to free it if necessary.

    ret = gp_widget_set_value (child, val);
    if (ret < GP_OK) {
        fprintf (stderr, "could not set widget value: %d\n", ret);
        goto out;
    }
    ret = gp_camera_set_single_config (camera, key, child, context);
    if (ret != GP_OK) {
        // This stores it on the camera again
        ret = gp_camera_set_config (camera, widget, context);
        if (ret < GP_OK) {
            fprintf (stderr, "camera_set_config failed: %d\n", ret);
            return ret;
        }
    }
out:
    gp_widget_free (widget);
    return ret;
}



//  This enables/disables the specific canon capture mode.
//
//  For non canons this is not required, and will just return
//  with an error (but without negative effects).

int DSLR::canon_enable_capture (Camera *camera, int onoff, GPContext *context) {
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret;

    ret = gp_camera_get_single_config (camera, "capture", &widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }

    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK) {
        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type) {
        case GP_WIDGET_TOGGLE:
        break;
    default:
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }
    // Now set the toggle to the wanted value
    ret = gp_widget_set_value (child, &onoff);
    if (ret < GP_OK) {
        fprintf (stderr, "toggling Canon capture to %d failed with %d\n", onoff, ret);
        goto out;
    }
    // OK
    ret = gp_camera_set_single_config (camera, "capture", widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_set_config failed: %d\n", ret);
        return ret;
    }
out:
    gp_widget_free (widget);
    return ret;
}


/*
statis void DSLR::errordumper(GPLogLevel level, const char *domain, const char *str, void *data) {
  printf("%s\n", str);
}
*/
// This seems to have no effect on where images go
void set_capturetarget(Camera *canon, GPContext *canoncontext) {
    int retval;
    printf("Get root config.\n");
    CameraWidget *rootconfig; // okay, not really
    CameraWidget *actualrootconfig;
    retval = gp_camera_get_config(canon, &rootconfig, canoncontext);
    actualrootconfig = rootconfig;
    printf("  Retval: %d\n", retval);
    printf("Get main config.\n");
    CameraWidget *child;
    retval = gp_widget_get_child_by_name(rootconfig, "main", &child);
    printf("  Retval: %d\n", retval);
    printf("Get settings config.\n");
    rootconfig = child;
    retval = gp_widget_get_child_by_name(rootconfig, "settings", &child);
    printf("  Retval: %d\n", retval);
    printf("Get capturetarget.\n");
    rootconfig = child;
    retval = gp_widget_get_child_by_name(rootconfig, "capturetarget", &child);
    printf("  Retval: %d\n", retval);
    CameraWidget *capture = child;
    const char *widgetinfo;
    gp_widget_get_name(capture, &widgetinfo);
    printf("config name: %s\n", widgetinfo );
    const char *widgetlabel;
    gp_widget_get_label(capture, &widgetlabel);
    printf("config label: %s\n", widgetlabel);
    int widgetid;
    gp_widget_get_id(capture, &widgetid);
    printf("config id: %d\n", widgetid);
    CameraWidgetType widgettype;
    gp_widget_get_type(capture, &widgettype);
    printf("config type: %d == %d \n", widgettype, GP_WIDGET_RADIO);
    printf("Set value.\n");
    // capture to ram should be 0, although I think the filename also plays into
    // it
    int one=1;
    retval = gp_widget_set_value(capture, &one);
    printf("  Retval: %d\n", retval);
    printf("Enabling capture to CF.\n");
    retval = gp_camera_set_config(canon, actualrootconfig, canoncontext);
    printf("  Retval: %d\n", retval);
}


    void DSLR::capture_to_file(Camera *canon, GPContext *canoncontext, char *fn) {
    int fd, retval;
    CameraFile *canonfile;
    CameraFilePath camera_file_path;

    printf("Capturing.\n");

    retval = gp_camera_capture(canon, GP_CAPTURE_IMAGE, &camera_file_path, canoncontext);
    printf("  Retval: %d\n", retval);

    printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

    fd = open(fn, O_CREAT | O_WRONLY, 0644);
    retval = gp_file_new_from_fd(&canonfile, fd);
    printf("  Retval: %d\n", retval);
    retval = gp_camera_file_get(canon, camera_file_path.folder, camera_file_path.name,
             GP_FILE_TYPE_NORMAL, canonfile, canoncontext);
    printf("  Retval: %d\n", retval);

    printf("Deleting.\n");
    retval = gp_camera_file_delete(canon, camera_file_path.folder, camera_file_path.name,
            canoncontext);
    printf("  Retval: %d\n", retval);

    gp_file_free(canonfile);
}

void DSLR::on_pushButton_clicked()
{


    Camera	*canon;
    int	i, retval;

// ????????????????????????????????????????????????
//	GPContext *canoncontext = sample_create_context();
    GPContext *canoncontext = gp_context_new();
    gp_camera_new(&canon);




//	gp_log_add_func(GP_LOG_ERROR, errordumper, NULL);

//     When I set GP_LOG_DEBUG instead of GP_LOG_ERROR above, I noticed that the
//     init function seems to traverse the entire filesystem on the camera.  This
//     is partly why it takes so long.
//     (Marcus: the ptp2 driver does this by default currently.)
//
    printf("Camera init.  Takes about 10 seconds.\n");
    retval = gp_camera_init(canon, canoncontext);
    if (retval != GP_OK) {
        printf("  Retval: %d\n", retval);
        return;
    }
// ??????????????????????????????????????????????????????????????????????????????
//	canon_enable_capture(canon, TRUE, canoncontext);
//    set_capturetarget(canon, canoncontext);


    printf("Taking 100 previews and saving them to snapshot-XXX.jpg ...\n");
    int canonFocus=-3;
    for (i=0;i<10;i++) {
        CameraFile *file;
        char output_file[32];

        fprintf(stderr,"preview %d\n", i);
        retval = gp_file_new(&file);
        if (retval != GP_OK) {
            fprintf(stderr,"gp_file_new: %d\n", retval);
            return;
        }


//        autofocus every 10 shots
//		if (i%10 == 9) {
//			camera_auto_focus (canon, canoncontext);
//			camera_auto_focus (canon, canoncontext,0);
//		} else {
//			camera_manual_focus (canon, (i/10-5)/2, canoncontext);
//		}



// Canon: successively focus from -3 to 3
//		camera_manual_focus (canon, canonFocus, canoncontext);
        if(canonFocus>3)
            canonFocus=-3;
        else
            canonFocus++;


    set_config_value_string (canon, "eoszoom", "10", canoncontext);
 //   set_config_value_string (canon, "eoszoom", "5", canoncontext);
 //   set_config_value_string (canon, "eoszoom", "5", canoncontext);



#if 0 // testcase for EOS zooming
        {
//			char buf[20];
//			if (i<10) set_config_value_string (canon, "eoszoom", "5", canoncontext);
            if (i==2)
            {
                set_config_value_string (canon, "eoszoom", "5", canoncontext);
                set_config_value_string (canon, "eoszoom", "10", canoncontext);
            }
//			sprintf(buf,"%d,%d",(i&0x1f)*64,(i>>5)*64);
//			fprintf(stderr, "%d - %s\n", i, buf);
//			set_config_value_string (canon, "eoszoomposition", buf, canoncontext);
        }
#endif
        retval = gp_camera_capture_preview(canon, file, canoncontext);
        if (retval != GP_OK) {
            fprintf(stderr,"gp_camera_capture_preview(%d): %d\n", i, retval);
            return;
        }
        sprintf(output_file, "snapshot-%03d.jpg", i);
        retval = gp_file_save(file, output_file);
        if (retval != GP_OK) {
            fprintf(stderr,"gp_camera_capture_preview(%d): %d\n", i, retval);
            return;
        }
        gp_file_unref(file);

//        sprintf(output_file, "image-%03d.jpg", i);
//            capture_to_file(canon, canoncontext, output_file);

    }
    gp_camera_exit(canon, canoncontext);
//    return 0;



}
