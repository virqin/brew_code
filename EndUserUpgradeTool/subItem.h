#ifndef SUB_ITEM_PACKET
#define SUB_ITEM_PACKET

#define EFS2_DIAG_HELLO     0 /* Parameter negotiation packet               */
#define EFS2_DIAG_QUERY     1 /* Send information about EFS2 params         */
#define EFS2_DIAG_OPEN      2 /* Open a file                                */
#define EFS2_DIAG_CLOSE     3 /* Close a file                               */
#define EFS2_DIAG_READ      4 /* Read a file                                */
#define EFS2_DIAG_WRITE     5 /* Write a file                               */
#define EFS2_DIAG_SYMLINK   6 /* Create a symbolic link                     */
#define EFS2_DIAG_READLINK  7 /* Read a symbolic link                       */
#define EFS2_DIAG_UNLINK    8 /* Remove a symbolic link or file             */
#define EFS2_DIAG_MKDIR     9 /* Create a directory                         */
#define EFS2_DIAG_RMDIR    10 /* Remove a directory                         */
#define EFS2_DIAG_OPENDIR  11 /* Open a directory for reading               */
#define EFS2_DIAG_READDIR  12 /* Read a directory                           */
#define EFS2_DIAG_CLOSEDIR 13 /* Close an open directory                    */
#define EFS2_DIAG_RENAME   14 /* Rename a file or directory                 */
#define EFS2_DIAG_STAT     15 /* Obtain information about a named file      */
#define EFS2_DIAG_LSTAT    16 /* Obtain information about a symbolic link   */
#define EFS2_DIAG_FSTAT    17 /* Obtain information about a file descriptor */
#define EFS2_DIAG_CHMOD    18 /* Change file permissions                    */
#define EFS2_DIAG_STATFS   19 /* Obtain file system information             */
#define EFS2_DIAG_ACCESS   20 /* Check a named file for accessibility       */
#define EFS2_DIAG_NAND_DEV_INFO    21 /* Get NAND device info               */
#define EFS2_DIAG_FACT_IMAGE_START 22 /* Start data output for Factory Image*/
#define EFS2_DIAG_FACT_IMAGE_READ  23 /* Get data for Factory Image         */
#define EFS2_DIAG_FACT_IMAGE_END   24 /* End data output for Factory Image  */
#define EFS2_DIAG_PREP_FACT_IMAGE  25 /* Prepare file system for image dump */
#define EFS2_DIAG_PUT      26 /* Write an EFS item file                     */
#define EFS2_DIAG_GET      27 /* Read an EFS item file                      */
#define EFS2_DIAG_ERROR    28 /* Semd an EFS Error Packet back through DIAG */
#define EFS2_DIAG_EXTENDED_INFO 29 /* Get Extra information.                */
#define EFS2_DIAG_CHOWN         30 /* Change ownership                      */
#define EFS2_DIAG_BENCHMARK_START_TEST  31 /* Start Benchmark               */
#define EFS2_DIAG_BENCHMARK_GET_RESULTS 32 /* Get Benchmark Report          */
#define EFS2_DIAG_BENCHMARK_INIT        33 /* Init/Reset Benchmark          */
#define EFS2_DIAG_SET_RESERVATION       34 /* Set group reservation         */
#define EFS2_DIAG_SET_QUOTA             35 /* Set group quota               */
#define EFS2_DIAG_GET_GROUP_INFO        36 /* Retrieve Q&R values           */

#define O_ACCMODE          0003
#define O_RDONLY             00
#define O_WRONLY             01
#define O_RDWR               02
#define O_CREAT            0100 /* not fcntl */
#define O_EXCL             0200 /* not fcntl */
#define O_NOCTTY           0400 /* not fcntl */
#define O_TRUNC           01000 /* not fcntl */
#define O_APPEND          02000
#define O_NONBLOCK        04000
#define O_NDELAY        O_NONBLOCK
#define O_SYNC           010000
#define FASYNC           020000 /* fcntl, for BSD compatibility */
#define O_DIRECT         040000 /* direct disk access hint */
#define O_LARGEFILE     0100000
#define O_DIRECTORY     0200000 /* must be a directory */
#define O_NOFOLLOW      0400000 /* don't follow links */
#define O_ITEMFILE     01000000 /* Create special ITEM file. */
#define O_AUTODIR      02000000 /* Allow auto-creation of directories */

typedef struct
{
	byte opaque_header[4];
}
diagpkt_subsys_header_type;

typedef  struct {
	diagpkt_subsys_header_type hdr;
	uint32 targ_pkt_window;  /* Window size in packets for sends from phone  */
	uint32 targ_byte_window; /* Window size in bytes for sends from phone    */
	uint32 host_pkt_window;  /* Window size in packets for sends from host   */
	uint32 host_byte_window; /* Window size in bytes for sends from host     */
	uint32 iter_pkt_window;  /* Window size in packets for dir iteration     */
	uint32 iter_byte_window; /* Window size in bytes for dir iteration       */
	uint32 version;          /* Protocol version number                      */
	uint32 min_version;      /* Smallest supported protocol version number   */
	uint32 max_version;      /* Highest supported protocol version number    */
	uint32 feature_bits;     /* Feature bit mask; one bit per feature        */
} efs2_diag_hello_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	uint32 targ_pkt_window;  /* Window size in packets for sends from phone  */
	uint32 targ_byte_window; /* Window size in bytes for sends from phone    */
	uint32 host_pkt_window;  /* Window size in packets for sends from host   */
	uint32 host_byte_window; /* Window size in bytes for sends from host     */
	uint32 iter_pkt_window;  /* Window size in packets for dir iteration     */
	uint32 iter_byte_window; /* Window size in bytes for dir iteration       */
	uint32 version;          /* Protocol version number                      */
	uint32 min_version;      /* Smallest supported protocol version number   */
	uint32 max_version;      /* Highest supported protocol version number    */
	uint32 feature_bits;     /* Feature bit mask; one bit per feature        */
} efs2_diag_hello_rsp_type;

/*
* Query Packet
*
* Used to return the values of EFS2 constants that might be of interest
* to the tool.
*/
typedef struct {
	diagpkt_subsys_header_type hdr;
} efs2_diag_query_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 max_name;          /* Maximum filename length                      */
	int32 max_path;          /* Maximum pathname length                      */
	int32 max_link_depth;    /* Maximum number of symlinks followed          */
	int32 max_file_size;     /* Maximum size of a file in bytes              */
	int32 max_dir_entries;   /* Maximum number of entries in a directory     */
	int32 max_mounts;        /* Maximum number of filesystem mounts          */
} efs2_diag_query_rsp_type;
/*
* Open File Packet
*
* Used to open a named file.
*
* The following oflag values are valid:
* 0x0000: O_RDONLY (open for reading mode)
* 0x0001: O_WRONLY (open for writing mode)
* 0x0002: O_RDWR   (open for reading and writing)
* 0x0002: O_TRUNC  (if successfully opened, truncate length to 0)
* 0x0100: O_CREAT  (create file if it does not exist)
*
* The O_CREAT flag can be orred with the other flags.
* The mode field is ignored unless the O_CREAT flag is specified.
* If O_CREAT is specified, the mode is a three-digit octal number with
* each octet representing read, write and execute permissions for owner,
* group and other.
*
* Note: If a request is received to open an already open file, the file
* is closed and reopened.
*/
typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 oflag;             /* Open flags                                   */
	int32 mode;              /* Mode                                         */
	char  path[1];           /* Pathname (null-terminated string)            */
} efs2_diag_open_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 fd;                /* File descriptor if successful, -1 otherwise  */
	int32 err_no;             /* Error code if error, 0 otherwise             */
} efs2_diag_open_rsp_type;

/*
* Close File Packet
*
* Used to close a file descriptor.
*
* Note: If a request is received to close an already closed file, nothing
* is done but an error is not reported.
*/
typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 fd;                /* File descriptor                              */
} efs2_diag_close_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 err_no;             /* Error code if error, 0 otherwise             */
} efs2_diag_close_rsp_type;

/*
* Read File Packet
*
* Used to read the contents of an open file. Note that the response packet
* does not explicitly contain the number of bytes read, since this can be
* computed from the packet size. The number of bytes actually read may be
* less than the number specified in the request packet if EOF is encountered
* (this is not an error).
*
* NOTE 1: The fd is part of the response packet because we want to allow for
* the possibility of more than one file being open for reading. The fd
* allows the tool to match a response with a request.
*
* NOTE 2: This is one of the packets that support windowing. Thus there can
* be several requests outstanding before a response is sent.
*/
typedef struct {
	diagpkt_subsys_header_type hdr;
	int32  fd;               /* File descriptor                              */
	uint32 nbyte;            /* Number of bytes to read                      */
	uint32 offset;           /* Offset in bytes from the origin              */
} efs2_diag_read_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	int32  fd;               /* File descriptor                              */
	uint32 offset;           /* Requested offset in bytes from the origin    */
	int32  bytes_read;       /* bytes read if successful, -1 otherwise       */
	int32  err_no;            /* Error code if error, 0 otherwise             */
	char   data[512];       /* The data read out                            */
} efs2_diag_read_rsp_type;

/*
* Write File Packet
*
* Used to write data into a given offset in an open file. If the offset is
* past the current end of the file, the file is zero-filled until the offset
* and then the data is written to the file. Note that the request packet
* does not explicitly contain the number of bytes to be written, since this
* can be computed from the packet size. Unless an error occurs, all the
* bytes specified in the data field of the request packet are written to
* the file.
*
* NOTE 1: The fd is part of the response packet because we want to allow for
* the possibility of more than one file being open for writing. The fd allows
* the tool to match a response with a request.
*
* NOTE 2: This is one of the packets that support windowing. Thus there can
* be several requests outstanding before a response is sent.
*/
typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 fd;                /* File descriptor                              */
	uint32 offset;           /* Offset in bytes from the origin            */
	int32  nlen;			// length to write
	char  data[512];           /* The data to be written                       */
} efs2_diag_write_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 fd;                /* File descriptor                              */
	uint32 offset;           /* Requested offset in bytes from the origin    */
	int32 bytes_written;     /* The number of bytes written                  */
	int32 err_no;             /* Error code if error, 0 otherwise             */
} efs2_diag_write_rsp_type;

/*
* Make Directory Packet
*
* Used to create a directory.
*
* Note: If request is received to create an existing directory, the
* directory will be removed if possible and recreated. If directory
* removal fails (because it is not empty, typically) then an error is
* returned.
*/
typedef struct {
	diagpkt_subsys_header_type hdr;
	int16 mode;              /* The creation mode                            */
	char  path[1];           /* Pathname (null-terminated string)            */
} efs2_diag_mkdir_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 err_no;             /* Error code if error, 0 otherwise             */
} efs2_diag_mkdir_rsp_type;

/*
* Remove Directory Packet
*
* Used to delete a directory. The deletion will fail if the directory is
* not empty.
*
* Note: If a request is received to remove an already removed directory,
* nothing is done but an error is not reported.
*/
typedef struct {
	diagpkt_subsys_header_type hdr;
	char  path[1];           /* Pathname (null-terminated string)            */
} efs2_diag_rmdir_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 err_no;             /* Error code if error, 0 otherwise             */
} efs2_diag_rmdir_rsp_type;

/*
* Open Directory Packet
*
* Used to open a directory for reading. Returns a directory pointer that
* can be used to read the contents of the directory.
*
* Note: If a request is received to open an already open directory, nothing
* is done but the dirp from the previous open is returned.
*/
typedef struct {
	diagpkt_subsys_header_type hdr;
	char  path[1];           /* Pathname (null-terminated string)            */
} efs2_diag_opendir_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	uint32 dirp;             /* Directory pointer. NULL if error             */
	int32 err_no;             /* Error code if error, 0 otherwise             */
} efs2_diag_opendir_rsp_type;

/*
* Read Directory Packet
*
* Used to read the next entry from an open directory. Returns the name
* of the entry and some information about it. The directory pointer passed
* in is returned in the response packet so that the tool can match it to
* the request packet. The request packet contains a sequence number so that
* the tool  can request to retrieve an entry again if the response packet
* does not get through. If the iteration is complete, the entry_name field
* in the response packet will contain an empty string (first character =
* '\0'). Valid sequence numbers are positive integers, with a sequence
* number of 1 indicating the first entry in the directory. "." and "..",
* which are entries present in every directory, are not passed back.
*
* NOTE: This is one of the packets that support windowing. Thus there can
* be several requests outstanding before a response is sent.
*/
typedef struct {
	diagpkt_subsys_header_type hdr;
	uint32 dirp;             /* Directory pointer.                           */
	int32 seqno;             /* Sequence number of directory entry to read   */
} efs2_diag_readdir_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	uint32 dirp;             /* Directory pointer.                           */
	int32  seqno;            /* Sequence number of directory entry           */
	int32  err_no;            /* Error code if error, 0 otherwise             */
	int32  entry_type;       /* 0 if file, 1 if directory, 2 if symlink      */
	int32  mode;             /* File mode                                    */
	int32  size;             /* File size in bytes                           */
	int32  atime;            /* Time of last access                          */
	int32  mtime;            /* Time of last modification                    */
	int32  ctime;            /* Time of last status change                   */
	char   entry_name[1];    /* Name of directory entry (not full pathname)  */
} efs2_diag_readdir_rsp_type;

/*
* Close Directory Packet
*
* Used to close an open directory.
*
* Note: If a request is received to close an already closed directory,
* nothing is done but an error is not returned.
*/
typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 dirp;              /* Directory pointer.                           */
} efs2_diag_closedir_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 err_no;             /* Error code if error, 0 otherwise             */
} efs2_diag_closedir_rsp_type;

/*
* Rename Packet
*
* Used to rename a file or directory. Note that duplicate requests to rename
* a directory will lead to an error code being returned the second time if
* the rename was successful the first time. If the duplicate request is due
* to a missing response packet, the tool needs to check the error code to
* make sure that the error is reasonable (ENOENT will be the error returned
* in this case).
*/
typedef struct {
	diagpkt_subsys_header_type hdr;
	char oldpath[1];         /* Old pathname (null-terminated string)        */
	char newpath[1];         /* New pathname (null-terminated string)        */
} efs2_diag_rename_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 err_no;             /* Error code if error, 0 otherwise             */
} efs2_diag_rename_rsp_type;

/*
* Fstat Packet
*
* Used to retrieve information about a file descriptor.
*/
typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 fd;                /* File descriptor                              */
} efs2_diag_fstat_req_type;

typedef struct {
	diagpkt_subsys_header_type hdr;
	int32 err_no;             /* Error code if error, 0 otherwise             */
	int32 mode;              /* File mode                                    */
	int32 size;              /* File size in bytes                           */
	int32 nlink;             /* Number of links                              */
	int32 atime;             /* Time of last access                          */
	int32 mtime;             /* Time of last modification                    */
	int32 ctime;             /* Time of last status change                   */
} efs2_diag_fstat_rsp_type;

typedef  union
{
	efs2_diag_hello_req_type	hello_req;
	efs2_diag_query_req_type	query_req;
	efs2_diag_open_req_type		file_open_req;
	efs2_diag_close_req_type	file_close_req;
	efs2_diag_read_req_type		file_read_req;
	efs2_diag_write_req_type	file_write_req;
	efs2_diag_mkdir_req_type	mkdir_req;
	efs2_diag_rmdir_req_type	rmdir_req;
	efs2_diag_opendir_req_type	open_dir_req;
	efs2_diag_readdir_req_type	read_dir_req;
	efs2_diag_closedir_req_type	close_dir_req;
	efs2_diag_rename_req_type	rename_req;
	efs2_diag_fstat_req_type	fstat_req;


}diag_sub_efs2_req_type;

typedef  union
{
	efs2_diag_hello_rsp_type	hello_rsp;
	efs2_diag_query_rsp_type	query_rsp;
	efs2_diag_open_rsp_type		file_open_rsp;
	efs2_diag_close_rsp_type	file_close_rsp;
	efs2_diag_read_rsp_type		file_read_rsp;
	efs2_diag_write_rsp_type	file_write_rsp;
	efs2_diag_mkdir_rsp_type	mkdir_rsp;
	efs2_diag_rmdir_rsp_type	rmdir_rsp;
	efs2_diag_opendir_rsp_type	open_dir_rsp;
	efs2_diag_readdir_rsp_type	read_dir_rsp;
	efs2_diag_closedir_rsp_type	close_dir_rsp;
	efs2_diag_rename_rsp_type	rename_rsp;
	efs2_diag_fstat_rsp_type	fstat_rsp;
}diag_sub_efs2_rsp_type;

 
#endif