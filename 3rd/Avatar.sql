

CREATE TABLE IF NOT EXISTS `Avatar`(
	`id_` BIGINT UNSIGNED AUTO_INCREMENT,
	`name_` VARCHAR(20) NOT NULL,
	`state_` TINYINT NOT NULL,
	`hp_` INT NOT NULL,
	`mp_` INT NOT NULL,
	`gold_` INT NOT NULL,
	
	`dirty_` TINYINT NOT NULL,
	
	`bag_` BLOB,
	`equip_` BLOB,
	`mission_` BLOB,
	
	`create_time` DATE NOT NULL,
	
	PRIMARY KEY(`id_`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
