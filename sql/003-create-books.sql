create table books (
    iid serial not null primary key,
    rid_catalogue int not null references catalogue(iid),
    author text,
    title text,
    eyear int,
    elocation text,
    publisher text,
    pages int,
    annote text,
    aconnent text
);
