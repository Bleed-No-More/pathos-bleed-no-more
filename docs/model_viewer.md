# Pathos Model Viewer

# Description
Pathos comes with it's own model viewer, which supports the VBM format. This model
viewer was based on the original Half-Life Model Viewer by Mete Ciragan, and uses
the Mx Toolkit for the UI.

Along with being able to view your models, the model viewer gives you the following
features:
 - Compiling models with a compile tab.
 - Animating facial flex scripts to a soun file played.
 - Rendering a skybox texture as a background.
 
Under Options->Settings, you can specify the basic settings of the tool, such as the
path to studiomdl, the arguments passed to studiomdl, the decompiler path, and the mod
folder used by the model viewer. If the last one is changed, you'll need to restart the
model viewer for the changes to take effect.

Under File->Compile QC, you will be able to access a tab which allows you to pick a QC
file to compile and/or edit. The tool associated with this can be set under 
View->File Assocations. I generally use Notepad++ for this purpose. You can also specify
a target directory, to which you can copy your compiled VBM and MDL files by pressing the
Copy button. I use this for moving my models from their source folders to the models
folder in the mod directory.

Another useful feature is the ability to load up a folder of models by going into 
File->Open Folder. This allows the model viewer to display a list of models in that folder
to view just by clicking on one model from the list twice. This will make it easier to
pick out prop models for your level.
Your recently opened folders will be available under File->Recent Folders.

> [!NOTE]
> While you can change texture flags under the Texture tab, these will not modify the PMF
> file contents. For that, you need to edit the PMF file with the appropriate settings and
> flags yourself. This is solely used for testing out the individual settings.

# Flex scripting
The model viewer allows you to do flex scripting for characters by loading their models
into the viewer, then going to the "Flex Scripting" tab. Here, you can load up a sound
file by pressing "Load Wav". The editor will then automatically set the duration, and a
time graph will be visible. You can alternatively set the duration yourself in the small
white reticle marked "Duration".

You can select a flex controller in the model from the lift on the left, and then either
use the buttons, or the mouse to edit the strengths of individual flexes along the time
graph. By double-clicking with the right mouse button, you can perform a bind. Clicking
the middle mouse button twice near the bind will delete the bind. If not using the mouse
to do binds, you need to specify the strength of the flex with the slider beneath the list
of flexes.

You can load up a script with "Load Script", or save a script with "Save Script". Pressing
"Reset" will completely clear the script editor. Setting "Loop" will mark the script to loop
indefinitely, otherwise the last flex state will remain active.

The green bar marks the space on the timegraph which will be overridden/affected if you bind
again, or delete a bind. Flex scripts should be saved under "pathos/scripts/expressions", in
subfolders for your specific NPCs.

For more information on facial flexes, check the "facial_animations.txt" file.