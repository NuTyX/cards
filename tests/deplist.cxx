#include <cstdio>
#include <cstdlib>

struct depList {
	unsigned int* depsIndex;
	int* level;
	unsigned int count;
	unsigned int decrement;
	int decount;
};
depList* initDepsList(void)
{
	depList* list;
	list = (depList*)malloc(sizeof *list );
	list->depsIndex = (unsigned int*)malloc(sizeof (*list->depsIndex ));
	list->level     = (int*)malloc(sizeof (*list->level));
	list->count     = 0;
	list->decount   = 0;
	list->decrement = 0;
	return list;
}
void addDepToDepList(depList *list, unsigned int nameIndex, int level)
{

	list->depsIndex = (unsigned int*)realloc(list->depsIndex,
		sizeof (*list->depsIndex ) * (list->count+1));
	if (list->depsIndex == NULL) {
		fprintf(stderr,"Failed to realloc %d\n",&list->depsIndex);
		exit(EXIT_FAILURE);
	}
	list->depsIndex[list->count] = nameIndex;

	list->level = (int*)realloc(list->level,
		sizeof (*list->level ) * (list->count+1));
	if (list->level == NULL) {
		fprintf(stderr,"Failed to realloc %d\n",&list->level);
		exit(EXIT_FAILURE);
	}

	list->level[list->count] = level;
	++list->count;
	list->decount = list->count;
}
void freeDepList(depList *list)
{
	return;
	if (list != nullptr) {
		for (unsigned int i = 0 ; i < list->count;i++) {
			if  (list->level != nullptr) {
				free(list->level);
				list->level = nullptr;
			}
			if (list->depsIndex != nullptr) {
				free(list->depsIndex);
				list->depsIndex = nullptr;
			}
		}
		free(list);
		list = nullptr;
	}
}
class dep_list {
		int m_decount;
		unsigned int m_decrement;
		unsigned int m_size;
		unsigned int m_capacity;

		int* m_level;
		unsigned int* m_depsIndex;

	public:
		dep_list()
			:m_decrement(0),m_decount(0) {
			m_size = 0;
			m_capacity = 1;
			m_depsIndex = (unsigned int*)malloc(sizeof (m_depsIndex));
			m_level     = (int*)malloc(sizeof (m_level));
		};
		~dep_list() {
			if (m_level != nullptr) {
				free(m_level);
				m_level = nullptr;
			}
			if (m_depsIndex != nullptr) {
				free(m_depsIndex);
				m_depsIndex = nullptr;
			}
		};
		void addDepToDepList(unsigned int nameIndex, int level) {
			if ( m_capacity == m_size)
				reserve(m_size * 2);
			m_level[m_size] = level;
			m_depsIndex[m_size] = nameIndex;
			++m_size;
			m_decount = m_size;
		}
		void reserve(unsigned int capacity) {
			if (m_capacity >= capacity)
				return;
			m_capacity = capacity;
			m_depsIndex = (unsigned int*)realloc(m_depsIndex,
				sizeof(m_depsIndex) * m_capacity);
			if ( m_depsIndex == nullptr ) {
				printf("Cannot reallocate m_items: %d", &m_depsIndex);
				return;
			}
			m_level = ( int*)realloc(m_level,
				sizeof(m_level)* m_capacity);
			if ( m_level == nullptr ) {
				printf("Cannot reallocate m_items: %d", &m_level);
				return;
			}
		}
		unsigned int size() {
			return m_size;
		}
		unsigned int capacity() {
			return m_capacity;
		}
		int decount() {
			return m_decount;
		}
		unsigned int decrement() {
			return m_decrement;
		}
		void decount(int decount) {
			m_decount = decount;
		}
		void decrement(unsigned int decrement) {
			m_decrement = decrement;
		}
		int level(int i) {
			return m_level[i];
		}
		unsigned int depsIndex(int i) {
			return m_depsIndex[i];
		}
		
};
int main() {
	depList* dependenciesList = initDepsList();
	addDepToDepList(dependenciesList, 1,0);
	addDepToDepList(dependenciesList, 10,1);
	addDepToDepList(dependenciesList, 20,2);
	addDepToDepList(dependenciesList, 30,3);
	addDepToDepList(dependenciesList, 40,4);
	dependenciesList->decrement = 100;
	dependenciesList->decount = 200;
	for (int i = 0; i <dependenciesList->count;i++)
		printf("%d; %d\n",
			dependenciesList->depsIndex[i],
			dependenciesList->level[i]);
	printf("count: %d, decount: %d, decrement: %d\n",
			dependenciesList->count,
			dependenciesList->decount,
			dependenciesList->decrement);

	printf("\n\n");
	dep_list dependencies_List;
	dependencies_List.addDepToDepList(1,0);
	dependencies_List.addDepToDepList(10,1);
	dependencies_List.addDepToDepList(20,2);
	dependencies_List.addDepToDepList(30,3);
	dependencies_List.addDepToDepList(40,4);
	dependencies_List.decrement(100);
	dependencies_List.decount(200);
	for (int i = 0; i < dependencies_List.size();i++)
		printf("%d; %d\n",
			dependencies_List.depsIndex(i),
			dependencies_List.level(i));

	printf("size: %d, capacity: %d, decount: %d, decrement: %d\n",
		dependencies_List.size(),
		dependencies_List.capacity(),
		dependencies_List.decount(),
		dependencies_List.decrement());
		

	return 0;
}
