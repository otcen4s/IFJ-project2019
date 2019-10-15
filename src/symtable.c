/* IFJ project
**@brief Hash table  
**
**@author: Lívia Žitňanská
*/

// Inicializacia tabulky
void hash_table_init(Hash_table *h_table)
{
    if (h_table==NULL)      //tabulka neexistuje
        return NULL;

    for (int i=0;i<n;i++) //nastavenie vsetkych prvkov na NULL
    {
        h_table[i]==NULL;
    }
}

//Hashovacia funkcia
unsigned int htab_hash_function(const char *str)
{
    uint32_t h=0; //musí byť 32 bitov
        const unsigned char *p;
        for (p=(const unsigned char*)str); *p!='\0'; p++)
            h=65599*h + *p;
        return h;
}

//Vkladanie prvku
bool hash_table_insertion(Hash_table *h_table, string str)
{
    int index = hash_function(str) //vypocitanie indexu cez hashovaciu funkciu
    //h_table[index].push_back(str) - vlozenie prvku na konkretny index

}

//Hladanie prvku
bool hash_table_searching (Hash_table *h_table, string str);
{
    int index = hash_function(str) //vypocitanie indexu cez hashovaciu funkciu
    
    for(int index =0;i<h_table[i])
    {
        if(h_table[index][i] == s) //nasli sme zhodny znak
        {
            //tu su volake bitove posunz, ktorym nerozumiem
            // cout << s << "is found!" << endl;
            return;
        }
    }
    // cout << s << "is found!" << endl; //nenaslo prvok
}

//Uvolnenie celej tabulky
void hash_table_free(Hash_table *h_table);
{
    if (h_table == NULL)
        return;

    //Doplnenie uvolnenia
}
