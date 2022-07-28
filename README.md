# Shared folder unlocker 4 Synology NAS
This programm will unlock shared folders in a synology NAS, by reading titles and passwords from a KeePass DB.

To use it, you have to replace the three variables at the top, pathToKPScript, pathToPasswordDB and keepassPrefix.
After that, you have to mark every entry in the KeePass Database with a prefix (in the title field).
Example: If your folder-name is projectfolder, and your prefix of choice is nas-prefix_, you have to to concatenate both to nas-prefix_projectfolder. Do this for every KeePass entry you want the programm to iterate over.

There are some limitations. Folder-name and passwords can't contain special characters, spaces or mutated vowels.

After replacing all of this, throw it in gcc and your good to go.
GCC-Example: gcc -o unlockfolders unlockfolders.c
