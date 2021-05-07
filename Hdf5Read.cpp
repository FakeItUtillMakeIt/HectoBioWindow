#include "Hdf5Read.h"
#include "qDebug"

const H5std_string FILE_NAME("C:\\Users\\LJ\\Desktop\\example.fast5");

void Hdf5Read::save_file(double* data, int row, int col, const char* file_path, const char* db_name) {
	//�ȴ���һ���ļ�д��
	const int data_rank = 2;
	hid_t file_id;
	file_id = H5Fcreate(file_path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	hsize_t dims[2];
	dims[0] = row;
	dims[1] = col;

	hid_t dataspace_id, dataset_id;
	dataspace_id = H5Screate_simple(data_rank, dims, NULL);
	dataset_id = H5Dcreate2(file_id, db_name, H5T_IEEE_F64BE, dataspace_id, H5P_DEFAULT,
		H5P_DEFAULT, H5P_DEFAULT);
	herr_t status;
	status = H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

	status = H5Sclose(dataspace_id);
	status = H5Sclose(dataset_id);
	status = H5Sclose(file_id);

	return;
}


bool Hdf5Read::read_file(double* data, int& row, int& col, const char* file_path, const char* db_name) {
	//H5File file(FILE_NAME, H5F_ACC_RDONLY);
	
	//��HDF5�ļ�
	hid_t file_id;
	herr_t status;
	file_id = H5Fopen(file_path,H5F_ACC_RDWR,H5P_DEFAULT);

	// �������ݼ��е����ݱ���
	hid_t dataset_id;    // ���ݼ������id
	// dataset_id = H5Dopen(groupλ��id,
	//                 const char *name, ���ݼ���
	//                    ���ݼ���������)
	dataset_id = H5Dopen(file_id, db_name, H5P_DEFAULT);


	// ������д�����ݼ�
	// herr_t ��ȡ״̬ = H5Dread(д��Ŀ�����ݼ�id,
	//                              �ڴ���������,
	//                       memory_dataspace_id, �����ڴ�dataspace�����е�ѡ��
	//                          - H5S_ALL: �ļ���dataspace�����ڴ�dataspace��file_dataspace_id�е�ѡ����Ϊ�ڴ�dataspace��ѡ��
	//                         file_dataspace_id, �����ļ���dataspace��ѡ��
	//                          - H5S_ALL: �ļ���datasapce��ȫ��������Ϊ���ݼ���dataspace�����ȫ��ά������
	//                        ����IO������ת������,
	//                          const void * buf, �ڴ��н������ݵ�λ��
	status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

	hid_t dspace = H5Dget_space(dataset_id);/** ��ȡ���ݼ���С��Ϣ */
	hsize_t dims[2];
	H5Sget_simple_extent_dims(dspace, dims, NULL);
	row = dims[0];
	col = dims[1];
	
	// �ر�dataset��ض���
	status = H5Dclose(dataset_id);
	//       status = H5Sclose(dataspace_id);

		   // �ر��ļ�����
	status = H5Fclose(file_id);
	return true;
	
}

