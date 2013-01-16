DELETE FROM DB.DBA.load_list;
ld_dir('/usr/share/virtuoso-opensource-6.1/vad', '*.n3', 'http://wordnet.okfn.gr/resource/');
select * from DB.DBA.load_list;
rdf_loader_run();