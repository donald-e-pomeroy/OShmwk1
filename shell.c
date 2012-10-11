#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>


char path[4096];

/*node code from http://www.macs.hw.ac.uk/~rjp/Coursewww/Cwww/linklist.html*/
struct list_node {
	char *directory;
	struct list_node *next;
};
typedef struct list_node node;

/*the head node of the linked list that stores the stack for pushd, popd, dirs */
node *head;


/*checks for the pushd command and executes it*/
int check_pushd(char *buffer)
{
	char pushd_buffer[4096];
	strcpy(pushd_buffer, buffer);

	char *command_array[2];

	char *pushd_buffer_tokenize = strtok(pushd_buffer, " ");
	int in = 0;
	while (pushd_buffer_tokenize != NULL) {
		command_array[in] = pushd_buffer_tokenize;
		in++;
		pushd_buffer_tokenize = strtok(NULL, " ");
	}

	if (strcmp(command_array[0], "pushd") == 0) {

		node *temp1;
		node *temp2;

		char *current_directory_buffer = (char *)malloc(4096 * sizeof(char *));
		getcwd(current_directory_buffer, 4096);

		temp1 = (node *)malloc(sizeof(node));
		temp1->directory = current_directory_buffer;

		temp2 = head;

		if (head == NULL) {
			head = temp1;
			head->next = NULL;
		} else {
			while (temp2->next)
				temp2 = temp2->next;

			temp1->next = NULL;
			temp2->next = temp1;
		}
		int cd_ret = chdir(command_array[1]);
		if (cd_ret == -1) {
			perror("Error: error in chdir");
			return -1;
		}

		return 1;
	}
	return 0;
}

/*checks for the dirs command and executes it*/
int check_dirs(char *buffer)
{
	char pushd_buffer[4096];
	strcpy(pushd_buffer, buffer);

	char *command_array[1];

	char *pushd_buffer_tokenize = strtok(pushd_buffer, " ");
	int in = 0;
	while (pushd_buffer_tokenize != NULL) {
		command_array[in] = pushd_buffer_tokenize;
		in++;
		if (in < 1)
			return 0;

		pushd_buffer_tokenize = strtok(NULL, " ");
	}
	if (strcmp(command_array[0], "dirs") == 0) {
		node *node;
		node = head;
		while (node != NULL) {
			printf(" %s\n", node->directory);
			node = node->next;
		}
		return 1;
	}

return 0;
}

int check_popd(char *buffer)
{
	char pushd_buffer[4096];
	strcpy(pushd_buffer, buffer);

	char *command_array[2];

	char *pushd_buffer_tokenize = strtok(pushd_buffer, " ");
	int in = 0;
	while (pushd_buffer_tokenize != NULL) {
		command_array[in] = pushd_buffer_tokenize;
		in++;
		pushd_buffer_tokenize = strtok(NULL, " ");
	}

	if (strcmp(command_array[0], "popd") == 0) {

		if (head == NULL)
			return 1;


		if (head->next == NULL) {
			int cd_ret = chdir(head->directory);
			if (cd_ret == -1) {
				perror("Error: error in changing directory");
				return -1;
			}
			return 1;
		} else {
			node *next_node = head;
			while (next_node->next->next != NULL)
				next_node = next_node->next;


			int cd_ret = chdir(next_node->next->directory);
			if (cd_ret == -1) {
				perror("Error: error in changing directories");
				return -1;
			}
			next_node->next = NULL;
		}

		return 1;
	}
	return 0;
}

/*checks for the cd command and performs it if it was entered*/
int cd_check(char *buffer)
{
	char *cd_buffer = (char *)malloc(sizeof(char *));
	strcpy(cd_buffer, buffer);
	char *cd_tokenized = strtok(cd_buffer, " ");
	char *cd_array[2];
	int in = 0;
	while (cd_tokenized != NULL) {

		cd_array[in] = cd_tokenized;
		in++;
		cd_tokenized = strtok(NULL, " ");
	}

	if (strcmp(cd_array[0], "cd") == 0) {

		int cd_ret = chdir(cd_array[1]);
		if (cd_ret == -1) {
			perror("Error: error in chdir");
			return -1;
		}
		return 1;
	}
	return 0;
}

/*returns the path variable*/
int path_check(char *buffer)
{

	char *path_buffer = (char *)malloc(sizeof(char *));
	strcpy(path_buffer, buffer);
	char *path_tokenized = strtok(path_buffer, " ");
	char *path_array[3];
	int in = 0;

	while (path_tokenized != NULL) {
		path_array[in] = path_tokenized;
		in++;
		path_tokenized = strtok(NULL, " ");

	}

	if ((strcmp(path_array[0], "path") == 0) && (in == 1)) {

		printf("%s\n", path);
		return 1;
	}

	return 0;
}

/*checks to see if the path + command was entered, if it was it updates the path */
int path_plus_check(char *buffer)
{

	char path_buffer[4096];
	strcpy(path_buffer, buffer);
	char *path_tokenized = strtok(path_buffer, " ");
	char *path_array[3];
	int in = 0;
	while (path_tokenized != NULL) {
		path_array[in] = path_tokenized;
		in++;
		path_tokenized = strtok(NULL, " ");

	}

	if (in != 3)
		return 0;

	if ((strcmp(path_array[0], "path") == 0) && (strcmp(path_array[1], "+") == 0)) {
		strcat(path, path_array[2]);
		strcat(path, ":");
		return 1;
	}
	return 0;
}

/*checks for the path - dir command and removes the directory from the path*/
int path_minus_check(char *buffer)
{
	char path_buffer[4096];
	strcpy(path_buffer, buffer);
	char *path_tokenized = strtok(path_buffer, " ");
	char *path_array[3];
	int in = 0;
	while (path_tokenized != NULL) {
		path_array[in] = path_tokenized;
		in++;
		path_tokenized = strtok(NULL, " ");
	}

	if (in != 3)
		return 0;

	if ((strcmp(path_array[0], "path") == 0) && (strcmp(path_array[1], "-") == 0)) {
		char path_buffer2[4096];
		strcpy(path_buffer2, path);
		char * path_token = (char *)malloc(sizeof(char *));
		path_token = strtok(path_buffer2, ":");
		int count = 0;
		while (path_token != NULL) {
			count++;
			path_token = strtok(NULL, ":");
		}

		char *path_arr[count];

		int index = 0;
		char path_buffer3[4096];
		strcpy(path_buffer3, path);
		path_token = strtok(path_buffer3, ":");
		while (path_token != NULL) {
			path_arr[index] = path_token;
			index++;
			path_token = strtok(NULL, ":");
		}

		index = 0;
		char new_path[4096];
		while (index < count) {
			if (strcmp(path_arr[index], path_array[2]) != 0) {
				strcat(new_path, path_arr[index]);
				strcat(new_path, ":");
			}
			index++;
		}
		strcpy(path, new_path);
		return 1;
	}
	return 0;

}

/*checks for the path associated with a command*/
char *get_command_path(char *command)
{
	char count_paths_buffer[4096];
	strcpy(count_paths_buffer, path);
	int path_number = 0;

	char *tokenized = strtok(count_paths_buffer, ":");
	while (tokenized != NULL) {
		path_number++;
		tokenized = strtok(NULL, ":");
	}

	char path_check_buffer[4096];
	strcpy(path_check_buffer, path);

	char *tokenize_path = strtok(path_check_buffer, ":");
	while (tokenize_path != NULL) {
		if ((fopen(strcat(tokenize_path, command), "r")) > 0)
			return tokenize_path;


		tokenize_path = strtok(NULL, ":");
	}

	perror("Error: Command not in any path");
	return NULL;
}



int main(int argc, char **argv)
{
	head = NULL;
	strcpy(path, "/bin/:");
	char buffer[4096];


	while (1) {

		printf("%s", "w4118_sh>");
		/*get input*/
		fgets(buffer, 4096, stdin);
		/*get rid of the newline character
		//http://forums.devshed.com/
		c-programming-42/removing-trailing-newline-in-c-45260.html*/
		if (buffer[strlen(buffer) - 1] == '\n')
			buffer[strlen(buffer) - 1] = '\0';


		/*checks for the exit command*/
		int exit_check = strcmp(buffer, "exit");
		if (exit_check == 0)
			break;


		/*checks for the cd command, path, path +, path -, pushd, popd, dirs commands*/
		while ((cd_check(buffer) == 1) || (path_check(buffer) == 1)
		|| (path_plus_check(buffer) == 1) || (path_minus_check(buffer) == 1)
		|| (check_pushd(buffer) == 1) || (check_dirs(buffer) == 1) || (check_popd(buffer) == 1)) {

			printf("%s", "w4118_sh>");
			fgets(buffer, 4096, stdin);

			if (buffer[strlen(buffer) - 1] == '\n')
				buffer[strlen(buffer) - 1] = '\0';


			if (strcmp(buffer, "exit") == 0)
				exit(0);
		}

		int pid = fork();



		if (pid == -1) {
			perror("Error: error in fork");
			exit(1);
		}
		if (pid != 0) {
			wait(NULL);

		} else {
			printf("reached the else\n");
			/*creates the array for the arguments*/
			int num_of_args = 0;


			char *buffer2 = (char *)malloc(sizeof(char *));
			strcpy(buffer2, buffer);
			char *tokenized = strtok(buffer2, " ");
			while (tokenized != NULL) {

				num_of_args++;
				tokenized = strtok(NULL, " ");
			}

			
			
			
			char *arguments[num_of_args+1];

			char *tokenized2 = strtok(buffer, " ");
			int index = 0;
			while (tokenized2 != NULL) {				
				arguments[index] = (char*)malloc(512*sizeof(char));
				arguments[index] = tokenized2;
				index++;
				tokenized2 = strtok(NULL, " ");
			}

			
			char *path_to_use = (char *)malloc(sizeof(char *));
			path_to_use = get_command_path(arguments[0]);
			
			arguments[num_of_args] = NULL;

			int ret = execv(path_to_use, arguments);
			if (ret == -1) {
				perror("Error: error in execv");
				exit(1);
			}
		}
	}
	return 0;
}
