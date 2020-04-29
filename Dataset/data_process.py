import os
import re


class data_process(object):
    """
    normalize data to the format following:
    [{1,2,4,5,6},
    {1,2,5},
    {4,5}]
    """

    def __init__(self, data_class: str = 'Groceries'):
        """
        :param data_class: 'Groceries' or 'UNIX_usage'
        """
        if data_class not in ['Groceries', 'UNIX_usage']:
            exit(-2)
        if data_class == 'Groceries':
            self.data_class = 0
        elif data_class == 'UNIX_usage':
            self.data_class = 1
        self.data = []
        self.id_dict = {}
        self.counter = 0

    def put_items(self, items):
        """
        :param items: list | set
        :return:
        """
        if len(items) == 0:
            return
        tmp_list = set()
        for item in items:
            item_id = self.counter
            if item in self.id_dict:
                item_id = self.id_dict[item]
            else:
                self.id_dict[item] = self.counter
                self.counter += 1
            tmp_list.add(item_id)
        self.data.append(tmp_list)

    def read_dataset(self, path: str):
        """
        :param path: str
        :return:
        """
        if self.data_class == 0:
            template = re.compile(r'"\d*","\{(.+)\}"')
            with open(os.path.join(path, 'Groceries.csv'), mode='r', encoding='utf8') as fp:
                line = fp.readline()
                while line:
                    result = template.match(line)
                    if result is not None:
                        items_str = result.group(1)
                        if items_str is not None:
                            items = items_str.split(',')
                            self.put_items(items)
                    line = fp.readline()
                fp.close()
        elif self.data_class == 1:
            template = re.compile(r'<\d*>')
            for subdir in ['USER0', 'USER1', 'USER2', 'USER3', 'USER4', 'USER5', 'USER6', 'USER7', 'USER8']:
                filename = os.path.join(path, subdir, 'sanitized_all.981115184025')
                with open(filename, mode='r', encoding='utf8') as fp:
                    line = fp.readline()
                    items = []
                    while line:
                        if len(line) >= 7 and line[:7] == '**SOF**':
                            items = []
                        elif len(line) >= 7 and line[:7] == '**EOF**':
                            self.put_items(items)
                        elif template.match(line) is None:
                            items.append(line[:-1])
                        line = fp.readline()
                    fp.close()

    def save_result(self, filename: str):
        """
        :param filename: str
        :return:
        """
        with open(filename, mode='w') as fp:
            for a_set in self.data:
                for item in a_set:
                    fp.write(str(item))
                    fp.write(',')
                fp.write('\n')
            fp.close()
        with open(filename + '.dict', mode='w') as fp:
            for name, val in self.id_dict.items():
                fp.write('{},{}\n'.format(val, name))
            fp.close()


if __name__ == '__main__':
    dp = data_process('Groceries')  # UNIX_usage Groceries
    dp.read_dataset('../Assignment2')
    dp.save_result('Groceries.set')
    dp = data_process('UNIX_usage')  # UNIX_usage Groceries
    dp.read_dataset('../Assignment2')
    dp.save_result('UNIX_usage.set')
